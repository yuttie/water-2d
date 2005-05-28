/*
 *******************************************************************************
 *
 *  main.c - Water-2d
 *
 *  Copyright  2004  Yuta Taniguchi
 *******************************************************************************
 */


/* <<< Changelog >>>
 * >>> Mon, 02 Aug 2004 21:11:01 +0900 <<<
 * New:  波紋の大きさを+-キーで動的に変えることが出来るようにした。
 * 
 * >>> 2004/07/22 00:08:50 <<<
 * Src:  計算部分のアルゴリズムにn/32768倍アルゴリズムを選択し、(2^n-1)/2^n倍アルゴリズムを削除した。
 * New:  減衰率を変更する起動時オプションを追加した。
 * 
 * >>> 2004/07/21 16:03:00 <<<
 * Fix:  水面を静める演算の割算部分で切捨てを行うように変更し、完全に波が静まるように修正した。
 * Src:  あちこちに有るメッセージをconst定数化して一ヵ所でまとめて定義する様にした。
 * New:  ESCキーでも終了できるようにした。
 * Src:  CPUID、MMX、SSE、SSE2命令の存在チェック関数を実装した。
 * 
 * <<< 2004/03/27 12:10:05 >>>
 * Src:  プログラムの設定を一つの構造体にまとめた。
 * Src:  main関数をすっきりさせた。
 * New:  フルスクリーンモードでの実行を指定する起動時オプションを追加した。
 * Fix:  屈折テーブルの使用するメモリ量を1/8で済むように修正。
 * New:  背景画像から幅と高さを決めていたのを止め、自由に大きさを指定できる起動時オプションを追加した。
 * New:  自動的に指定された背景画像のサイズを、ウィンドウのサイズにリサイズするようにした。
 * New:  ドラッグ時に行う補間の分割数を指定する起動時オプションを追加した。
 * Fix:  水面の高さの解像度を変えると、屈折がおかしくなるのを修正。
 * Fix:  波紋の大きさを変えると、屈折がおかしくなるのを修正。
 * New:  水面の高さの解像度を直接指定できる起動時オプションを追加した。
 * Mod:  オプションの新規追加に従ってヘルプを更新。
 * New:  キャッシュ作成時に進行状況をタイトルバーに表示するようにした。
 * Fix:  屈折テーブルの計算が低精度だったのを修正。
 *
 * <<< 2004/02/28 15:55:20 >>>
 * Src:  サーフェスの拡大縮小関数を実装した。但し、実装だけ。
 *
 * <<< 2004/02/27 14:28:17 >>>
 * New:  ヘルプを表示する起動時オプションを加えた。
 *
 * <<< 2004/02/27 08:25:23 >>>
 * New: ドラッグ時の波紋の発生を補間付きにした
 * New: qキーで終了できるようにした
 *
 * <<< 2004/02/27 08:04:06 >>>
 * Fix:  波紋の加算により水面が最高点で飽和し平坦化する現象を直した。
 *
 * <<< ????/??/?? ??:??:?? >>>
 * Info: Windows + Delphiにより、Javaによる実装などを参考にして作成した。
 * Info: GDIを使い、MMXも使わず、非常に性能が悪かった。
 * Info: その後、Delphiのインラインアセンブラを利用して、主要計算部分をMMX化した。
 * Info: DelphiからC言語(Mingw)に変更するのを機に、SDLを利用することにした。
 * Info: 波紋の形を予め計算しキャッシュ化しておくことで、高速化を行った。
 * Info: 「屈折によるずれ」の計算をきちんと行い、キャッシュ化しておくことにした。
 * Info: フレネル反射を実装したが、上方から当たる白い光の反射を計算するだけなので、
 *       それほど綺麗では無く、むしろ無い方が良いと判断したので、削除した。
 */


/* <<< What I'm Doing >>>
 */


/* <<< To Do >>>
 * ** 解像度の最大値。
 * ** リファクタリングをする。
 * 70 波の反射を自由端反射にする。
 * 70 屈折テーブルキャッシュをファイルから読み込める様にする。
 * 70 PaintToSurface関数内で拡大して描画する案の実験
 * 60 vorboseモード
 * 50 キャッシュを使わないモード
 * 40 壁エディタ(円、直線、ピクセル単位の編集)
 * 30 SSE等、他のマルチメディア命令のサポート
 * 30 CPUマルチメディア命令のサポート状況によって処理を切替える
 * 20 画面に情報を表示(FPS等)
 * 10 他のディスプレイモードサポート
 */


/* <<< Max FPS(400x300) - debug >>>
 * 55.34
 * 55.39 - inline
 */


/* <<< Idea >>>
 * FPSの最大値を指定できるようにする
 * -vオプション指定で起動時にMaxWaterHeightなどのシミュレーションで使われる数値をコンソールに表示する
 * 背景の画像の対応形式を増やす
 * 
 */


/* <<< Specification >>>
 * ARGB-32bitのみサポート
 */


/* Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "SDL/SDL.h"
#include "imgscale.h"
#include "cpuidutil.h"
#include "calcfunc.h"
#include "main.h"


/* Constant */
const double PI = 3.1415926535;    // 円周率
// 計算の都合で本来の最大値65536の1/4しか利用できない
const int MAX_RESOLUTION = 65536 / 4;


/* TextData */
const char const *MSG_ERROR_OPTION =
    "ERROR: Invalid option '%s' was specified.\n";
const char const *MSG_ERROR_LOAD_IMAGE =
    "ERROR: Can't load image '%s'.\n";
const char const *MSG_ERROR_INIT_SDL =
    "ERROR: Initialization of SDL failed : %s\n";
const char const *MSG_ERROR_INIT_VIDEO =
    "ERROR: Initialization of video failed: %s\n";
const char const *MSG_HELP =
    "<usage>\n"
    "    water [option]\n"
    "<Options>\n"
    "    ** MemorySize(-mN) = HeightResolution(-pN)^2 + 2byte\n"
    "    ** '-rN' is equal to '-r N'\n"
    "    -h                : Show this help.\n"
    "    -f                : Use full-screen mode.\n"
    "    -aR(=0.99)        : Set rate of attenuation as R(0 < R < 1).\n"
    "    -bN(=20.0)        : Set depth of base ripple as N.\n"
    "    -rN(=8)           : Set ripple radius as N.\n"
    "    -dN(=1)           : Set number of interpolating division as N(0 < N).\n"
    "    -iF(=bgimage.bmp) : Use F as background image.\n"
    "    -sWxH(=256x192)   : Simulate water of the size of WxH.\n"
    "    -mN(=512)         : Use N KByte cache. Affect '-p' option.\n"
    "    -pN(=512)         : Set depth resolution as N. Affect '-m' option.\n";


/* Global Variable */
ProgConfig   g_Conf;         // プログラムの設定
PosData     *g_pNextData;    // 次の水面データ
PosData     *g_pCrntData;    // 今の水面データ
PosData     *g_pPrevData;    // 前の水面データ
PosData     *g_pRiplData;    // 現在の波紋データへのポインタ
Uint16      *g_pRfraTbl;     // 屈折による変移量テーブル
SDL_Surface *g_pScreen;      // スクリーンサーフェス
SDL_Surface *g_pBgImage;     // 背景サーフェス
void (*pCalcFunc)();         // 水面データ更新関数


/* Main Function */
int main(int argc, char **argv)
{
    // プログラム初期化
    InitProc(argc, argv);
    
    // FPS計測用変数
    Uint32 startTick = SDL_GetTicks();
    Uint32 endTick = 0;
    Uint32 frameCount = 0;
    
    // 無限ループ
    while(!EventProc())
    {
        // 水面をスクリーンサーフェスに描画する
        PaintToSurface(g_pScreen);

        // 表示を更新
        SDL_UpdateRect(g_pScreen, 0, 0, 0, 0);
        
        // 水面データを入れ替える
        PosData *tmp = g_pPrevData;
        g_pPrevData = g_pCrntData;
        g_pCrntData = g_pNextData;
        g_pNextData = tmp;
        
        // 水面を計算
        pCalcFunc();
        
        // FPS計測
        endTick = SDL_GetTicks();
        frameCount++;
        if (endTick - startTick > 1000)
        {
            // ウィンドウキャプションに表示するための文字列を作成
            char cap[16];
            sprintf(cap, "FPS : %#.2f", frameCount * 1000.0 / (endTick - startTick));
            // 文字列をウィンドウキャプションに設定
            SDL_WM_SetCaption(cap, NULL);
            // スタート時刻・フレーム数のリセット
            startTick = SDL_GetTicks();
            frameCount = 0;
        }
    }
    
    // プログラム終了処理
    ExitProc();
    
    return 0;
}


/* プログラム初期化関数 */
void InitProc(int argc, char **argv)
{
    // プログラムオプションのデフォルト値を設定
    g_Conf.pBgImgPath = "bgimage.bmp";
    g_Conf.depthRes = 512;
    g_Conf.riplRadius = 5;
    g_Conf.riplDepth = 20.0;
    g_Conf.widthRes = 256;
    g_Conf.heightRes = 192;
    g_Conf.wndWidth = g_Conf.widthRes;
    g_Conf.wndHeight = g_Conf.heightRes;
    g_Conf.attRate = 0.99;
    g_Conf.scale = g_Conf.depthRes / 200.0;
    g_Conf.csrIPDiv = 1;
    g_Conf.isFullScreen = false;
    
    // 引数解析
    if (ParseArgument(argc, argv))
    {
        // 冗長モード時 : 設定値を標準出力に出力する
        printf("BgImagePath : %s\n", g_Conf.pBgImgPath);
        printf("DepthResolution : %d\n", g_Conf.depthRes);
        printf("RippleRadius : %d\n", g_Conf.riplRadius);
        printf("RippleDepth : %f\n", g_Conf.riplDepth);
        printf("WidthResolution : %d\n", g_Conf.widthRes);
        printf("HeightResolution : %d\n", g_Conf.heightRes);
        printf("WindowWidth : %d\n", g_Conf.wndWidth);
        printf("WindowHeight : %d\n", g_Conf.wndHeight);
        printf("AttenuationRate : %f\n", g_Conf.attRate);
        printf("InterpolatingDivide : %d\n", g_Conf.csrIPDiv);
        printf("FullScreen : %s\n", g_Conf.isFullScreen ? "ON" : "OFF");
        /*
        printf("CPUID : %s\n", CheckCPUID() ? "Supported" : "Not Supported");
        printf("MMX : %s\n", CheckMMX() ? "Supported" : "Not Supported");
        printf("SSE : %s\n", CheckSSE() ? "Supported" : "Not Supported");
        printf("SSE2 : %s\n", CheckSSE2() ? "Supported" : "Not Supported");
        printf("EDX : %d\n", GetEDX());
        */
    }
    // CPUIDから水面データを更新する関数を決定
    pCalcFunc = CalculateMMX;

    // SDL初期化してスクリーンサーフェスを取得
    InitSDL();
    
    // 背景画像読み込み
    SDL_Surface *tmpBg = SDL_LoadBMP(g_Conf.pBgImgPath);
    if (tmpBg == NULL)
    {
        // エラー表示をして終了
        fprintf(stderr, MSG_ERROR_LOAD_IMAGE, g_Conf.pBgImgPath);
        exit(EXIT_FAILURE);
    }
    
    // 背景サーフェスをスクリーンサーフェスと同じ形式に変換
    tmpBg = SDL_ConvertSurface(tmpBg, g_pScreen->format, SDL_SWSURFACE);
    
    // 読み込んだ背景を水面の大きさに合わせる
    g_pBgImage = SDL_CreateRGBSurface(SDL_SWSURFACE,
            g_Conf.widthRes, g_Conf.heightRes, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    ScaleCopySurface(SM_BI_LINEAR, tmpBg, g_pBgImage);
    
    // 水面データ配列作成
    int arraySize = (1 + g_Conf.widthRes + 1) * (1 + g_Conf.heightRes + 1);
    g_pNextData = calloc(arraySize, sizeof(PosData));
    g_pCrntData = calloc(arraySize, sizeof(PosData));
    g_pPrevData = calloc(arraySize, sizeof(PosData));
    
    // 波紋データ作成
    g_pRiplData = CreateRippleData();
    
    // 屈折テーブルを作成
    g_pRfraTbl = CreateRefractionTable();
    
    // 屈折テーブルアクセス時に必要なオフセットを加算しておく
//    g_pRfraTbl += (g_Conf.depthRes * (MaxWaterHeight - MinWaterHeight + 1)) - MinWaterHeight;
}


/* イベント処理関数 */
bool EventProc()
{
    int topLimit = g_Conf.riplRadius;
    int bottomLimit = g_Conf.heightRes - g_Conf.riplRadius;
    int leftLimit = g_Conf.riplRadius;
    int rightLimit = g_Conf.widthRes - g_Conf.riplRadius;
    bool exitFlag = false;    // 終了要求が有るか否か
    static int preCsrX;    // 前のイベント発生時のマウスカーソルのX座標
    static int preCsrY;    // 前のイベント発生時のマウスカーソルのY座標
    SDL_Event event;          // イベントに関する情報が入る構造体(共用体?)
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            // キーダウンイベント
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
            case SDLK_q:
                // 終了キー(ESC, Q)
                exitFlag = true;
                break;
            case SDLK_KP_PLUS:
            case SDLK_PLUS:
                // +キー
                g_Conf.riplRadius += 4;
                if (g_Conf.riplRadius < 0) g_Conf.riplRadius = 0;
                free(g_pRiplData);
                g_pRiplData = CreateRippleData();
                break;
            case SDLK_KP_MINUS:
            case SDLK_MINUS:
                // -キー
                g_Conf.riplRadius -= 4;
                if (g_Conf.riplRadius < 0) g_Conf.riplRadius = 0;
                free(g_pRiplData);
                g_pRiplData = CreateRippleData();
                break;
            default:
                // その他のキー(ハンドルされないキー)
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            // マウスボタンダウンイベント
            // マウスのボタンが押されている時
            if (event.button.state == SDL_PRESSED)
            {
                // 波紋発生可能領域にいるかどうかをチェック
                if ((leftLimit < event.button.x) && (event.button.x < rightLimit) &&
                    (topLimit < event.button.y) && (event.button.y < bottomLimit))
                {
                    // 波紋を発生
                    RippleOut(event.button.x, event.button.y);
                    preCsrX = event.motion.x;
                    preCsrY = event.motion.y;
                }
            }
            break;
        case SDL_MOUSEMOTION:
            // マウスカーソル移動イベント
            // マウスのボタンが押されている時
            if (event.motion.state == SDL_PRESSED)
            {
                // 波紋発生可能領域にいるかどうかをチェック
                if ((leftLimit < event.motion.x) && (event.motion.x < rightLimit) &&
                    (topLimit < event.motion.y) && (event.motion.y < bottomLimit))
                {    // 波紋を発生

                    for (int i = 0; i < g_Conf.csrIPDiv; i++)
                    {
                        RippleOut((event.motion.x * i + preCsrX * (g_Conf.csrIPDiv - i)) / g_Conf.csrIPDiv,
                                  (event.motion.y * i + preCsrY * (g_Conf.csrIPDiv - i)) / g_Conf.csrIPDiv);
                    }
                    preCsrX = event.motion.x;
                    preCsrY = event.motion.y;
                }
            }
            break;
        case SDL_QUIT:
            // 終了イベント
            exitFlag = true;
            break;
        }
    }
    return exitFlag;
}


/* プログラム終了前の後始末 */
void ExitProc()
{
    // 屈折テーブルアクセス時に必要なオフセットを減算しておく
//    g_pRfraTbl -= (-(MinWaterHeight - MaxWaterHeight) * (MaxWaterHeight - MinWaterHeight + 1)) - MinWaterHeight;
    
    // メモリの解放
    SDL_FreeSurface(g_pBgImage);
    free(g_pNextData);
    free(g_pCrntData);
    free(g_pPrevData);
    free(g_pRiplData);
    free(g_pRfraTbl);
    
    // SDL終了
    SDL_Quit();
}


/* 起動時引数解析関数 */
bool ParseArgument(int argc, char **argv)
{
    bool isVerboseMode = false;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
            case 'h':
                // 使い方を表示して終了
                printf(MSG_HELP); exit(EXIT_SUCCESS);
                break;
            case 'v':
                // 冗長な表示を行う
                isVerboseMode = true;
                break;
            case 'f':
                // フルスクリーンモードに設定
                g_Conf.isFullScreen = true;
                break;
            case 'a':
                // 波紋の減衰率を取得
                if (argv[i][2] == '\0') g_Conf.attRate = atof(&argv[++i][0]);
                else                    g_Conf.attRate = atof(&argv[i][2]);
                break;
            case 'b':
                // 一倍の波紋の最大の深さ
                if (argv[i][2] == '\0') g_Conf.riplDepth = atof(&argv[++i][0]);
                else                    g_Conf.riplDepth = atof(&argv[i][2]);
                break;
            case 'r':
                // 波紋の大きさを取得
                if (argv[i][2] == '\0') g_Conf.riplRadius = atoi(&argv[++i][0]);
                else                    g_Conf.riplRadius = atoi(&argv[i][2]);
                break;
            case 'd':
                // 補間時の分割数を取得
                if (argv[i][2] == '\0') g_Conf.csrIPDiv = atoi(&argv[++i][0]);
                else                    g_Conf.csrIPDiv = atoi(&argv[i][2]);
                break;
            case 'i':
                // 背景のファイル名を取得
                if (argv[i][2] == '\0') g_Conf.pBgImgPath = &argv[++i][0];
                else                    g_Conf.pBgImgPath = &argv[i][2];
                break;
            case 's':
                // 計算上の水面のサイズを取得
                if (argv[i][2] == '\0')
                {
                    char *endPtr;
                    g_Conf.widthRes = (int)strtoul(&argv[++i][0], &endPtr, 10);
                    g_Conf.heightRes = (int)strtoul(++endPtr, NULL, 10);
                }
                else
                {
                    char *endPtr;
                    g_Conf.widthRes = (int)strtoul(&argv[i][2], &endPtr, 10);
                    g_Conf.heightRes = (int)strtoul(++endPtr, NULL, 10);
                }
                g_Conf.wndWidth = g_Conf.widthRes;
                g_Conf.wndHeight = g_Conf.heightRes;
                break;
            case 'm':
            {
                // メモリ使用可能量(KB単位)を取得
                // 水面の高さの精度(解像度)に影響する
                int mem;
                if (argv[i][2] == '\0') mem = atoi(&argv[++i][0]);
                else                    mem = atoi(&argv[i][2]);
                
                // 水面の高さに換算し、補正
                // 波の計算時に、上下左右を足すから
                // PosData(Sint16)の最大値32767/4=8191より
                // 各ピクセルの高さの最大値は8191、最小値は-8192
                // 解像度の最大値は8191 - (-8192) + 1=16384
                // 高低差の最小値は0、最大値は8191-(-8192)=16383
                // つまり、これは解像度-1に等しい。
                // 屈折テーブル用メモリの確保量は
                // 確保量=g_Conf.depthRes^2 * 2byte(16bit))
                g_Conf.depthRes = (int)(sqrt(mem * 1024 / 2.0));

                // 指定された解像度が最大値を越えていたら修正する。
                if (g_Conf.depthRes > MAX_RESOLUTION)
                    g_Conf.depthRes = MAX_RESOLUTION;
                
                //
//                    printf("%d\n", g_Conf.depthRes);
                
                // 波紋の高さを再計算
                //g_Conf.riplDepth = (g_Conf.depthRes - 1) / 2;
                
                break;
            }
            case 'p':
                // 水面の高さの精度(解像度)を取得
                // メモリ使用量に影響する
                if (argv[i][2] == '\0') g_Conf.depthRes = atoi(&argv[++i][0]);
                else                    g_Conf.depthRes = atoi(&argv[i][2]);
                
                // 指定された解像度が最大値を越えていたら修正する。
                if (g_Conf.depthRes > MAX_RESOLUTION)
                    g_Conf.depthRes = MAX_RESOLUTION;
                
                // 波紋の高さを再計算
                //g_Conf.riplDepth = (g_Conf.depthRes - 1) / 2;
                
                break;
            default:
                // 不正な引数が指定された場合、エラー表示をして終了
                fprintf(stderr, MSG_ERROR_OPTION, argv[i]);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // 不正な引数が指定された場合、エラー表示をして終了
            fprintf(stderr, MSG_ERROR_OPTION, argv[i]);
            exit(EXIT_FAILURE);
        }
    }
    
    // 冗長な表示を行うか否かを返す
    return isVerboseMode;
}


/* SDL初期化関数 */
void InitSDL()
{
    // SDLを初期化
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        // エラー表示をして終了
        fprintf(stderr, MSG_ERROR_INIT_SDL, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    // ディスプレイデバイスの初期化フラグを設定
    Uint32 screenFlags = SDL_SWSURFACE;
    if (g_Conf.isFullScreen) screenFlags |= SDL_FULLSCREEN;
    
    // ディスプレイデバイスを初期化
    g_pScreen = SDL_SetVideoMode(g_Conf.wndWidth, g_Conf.wndHeight, 32, screenFlags);
    
    // 初期化に失敗したら、エラーメッセージを表示して終了
    if (g_pScreen == NULL)
    {
        // エラー表示をして終了
        fprintf(stderr, MSG_ERROR_INIT_VIDEO, SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}


/* 波紋データ作成関数 */
PosData *CreateRippleData()
{
    int radius = g_Conf.riplRadius;
    // 波紋データ作成
    PosData *riplData = calloc((2 * radius + 1) * (2 * radius + 1), sizeof(PosData));
    PosData *pData = riplData;
    
    for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            // x = 0, y = 0を中心とする
            // 中心からの距離の二乗を求める
            int r2 = (x * x) + (y * y);
            
            if (r2 < radius * radius)
            {   // 円内
                double t = PI * (sqrt(r2) / radius);
                *pData = (PosData)((cos(t) + 1) * (g_Conf.depthRes / 2) * (g_Conf.riplDepth / 100.0) * (g_Conf.riplRadius / 8.0) / 2);
            }
            else
            {   // 円外
                *pData = 0;
            }
            pData++;
        }
    }
    return riplData;
}


/* 屈折テーブル作成関数 */
Uint16 *CreateRefractionTable()
{
/*
    // 512をデフォルトの分解能とすると、
    // 縦方向の分解能がp倍になると、波紋の高さもp倍になる
    // ここでピクセルの幅をp倍にしないと、p倍縦長の波紋になってしまう。
    // それでは困るので、ピクセルの幅をp倍する。
    // ここまでで、仮想的にp倍のスケールで計算した事になる。
    // そこで、算出したずれを1/p倍する。
    const double p = g_Conf.depthRes / 512.0;
    
    // 8をデフォルトの半径とすると、
    // 波紋の半径がq倍になると、波紋の高さはそのままなので、
    // q倍横長の波紋になってしまう。
    // そこで、波紋の高さをq倍する。
    // このまま計算すると、q倍のスケールで計算した事になる。
    // よって、算出したずれを1/q倍する。
    const double q = g_Conf.riplRadius / 8.0;
*/    
    const double PIXEL_WIDTH = 1.0;     // ピクセルの幅
    const double REFRACTION_INDEX = 1.33;    // 水の絶対屈折率

    // テーブル用メモリを確保(メモリ使用量≒g_Conf.depthRes^2 * 2byte(16bit))
    // ずれは、水面の高さと、そのピクセルの左右または上下の高低差に影響される
    // Left < Right
    Uint16 *refractionTable = calloc(g_Conf.depthRes * g_Conf.depthRes, sizeof(Uint16));
    
    // 計算開始
    int i = 0, d = g_Conf.depthRes / 10;
    for (int heightDiff = 0; heightDiff < g_Conf.depthRes; heightDiff++)
    {
        // 入射角を求める
        double t1 = atan((heightDiff * 100.0 / (g_Conf.depthRes / 2.0)) / (2.0 * PIXEL_WIDTH));
        
        // 屈折角を求める
        double t2 = asin(sin(t1) / REFRACTION_INDEX);
        
        // 各水面の高さに応じたずれの量を求める
        // ずれは、水面の高さと、そのピクセルの左右または上下の高低差に影響される
        for (int height = 0; height < g_Conf.depthRes; height++)
        {
            int index = g_Conf.depthRes * heightDiff + height;
            double delta = (height * 100.0 / (g_Conf.depthRes / 2.0)) * tan(t1 - t2);
            refractionTable[index] = (Uint16)(delta);
        }
        if (i < heightDiff)
        {
            char cap[32];
            sprintf(cap, "Creating Cache : %#.2f%%...", 100.0 * heightDiff / g_Conf.depthRes);
            SDL_WM_SetCaption(cap, NULL);
            i += d;
        }
    }
    
    // テーブルへのポインタを返す
    return refractionTable;
}



/* 波紋発生関数 */
void RippleOut(int x, int y)
{
    // 波データの左上隅にあたる位置に移動
    int incValue = (g_Conf.widthRes + 2) * ((y + 1) - g_Conf.riplRadius) +
                                           ((x + 1) - g_Conf.riplRadius);
    PosData *nextData = g_pNextData;
    PosData *crntData = g_pCrntData;
    nextData += incValue;
    crntData += incValue;
    PosData *riplData = g_pRiplData;
    
    // 波紋を発生させる
    incValue = (g_Conf.widthRes + 2) - (2 * g_Conf.riplRadius + 1);
    for (int iy = -g_Conf.riplRadius; iy <= g_Conf.riplRadius; iy++)
    {
        for (int ix = -g_Conf.riplRadius; ix <= g_Conf.riplRadius; ix++)
        {
            if (*riplData != 0)
            {
                // 異符号なら、普通に足す
                if (((*nextData >= 0) && (*riplData < 0)) || ((*nextData <= 0) && (*riplData > 0)))
                    *nextData += *riplData;
                // 同符号で波紋の高さより低いなら、そこまで引き上げる
                else if (*nextData < *riplData)
                    *nextData = *riplData;
                // 同符号で波紋の高さより大きいか同じであるなら、何もしない
                else
                    ;
                
                // 異符号なら、普通に足す
                if (((*crntData >= 0) && (*riplData < 0)) || ((*crntData <= 0) && (*riplData > 0)))
                    *crntData += *riplData;
                // 同符号で波紋の高さより低いなら、そこまで引き上げる
                else if (*crntData < *riplData)
                    *crntData = *riplData;
                // 同符号で波紋の高さより大きいか同じであるなら、何もしない
                else
                    ;
            }
            
            // ポインタを進める
            riplData++;
            nextData++;
            crntData++;
        }
        // 次に書きこむ行の頭に移動
        nextData += incValue;
        crntData += incValue;
    }
}


/* 水面描画関数 */
void PaintToSurface(SDL_Surface *target)
{
    // スクリーンサーフェスをロック
    SDL_LockSurface(target);
    SDL_LockSurface(g_pBgImage);
    
    // 最初のピクセルへ移動
    PosData *waterData = g_pNextData;
    waterData += (g_Conf.widthRes + 2) + 1;
    
    // 最初の行へのポインタで初期化
    Uint32 *scrLine = (Uint32 *)target->pixels;
    Uint32 *bgLine = (Uint32 *)g_pBgImage->pixels;
    
    // ピッチを取得
    Uint32 scrPitch = target->pitch / sizeof(Uint32);
    Uint32 bgPitch = g_pBgImage->pitch / sizeof(Uint32);
    
    // 描画
    for (int y = 0; y < g_Conf.heightRes; y++)
    {
        for (int x = 0; x < g_Conf.widthRes; x++)
        {
            int heightDiff;
            int dx;
            int dy;
            // X軸方向のずれを計算
            heightDiff = waterData[x + 1] - waterData[x - 1];
            if (heightDiff >= 0)
                dx =  g_pRfraTbl[ heightDiff * g_Conf.depthRes + ((g_Conf.depthRes - 1) / 2 + waterData[x])];
            else
                dx = -g_pRfraTbl[-heightDiff * g_Conf.depthRes + ((g_Conf.depthRes - 1) / 2 + waterData[x])];
            
            // Y軸方向のずれを計算
            heightDiff = waterData[x + (g_Conf.widthRes + 2)] - waterData[x - (g_Conf.widthRes + 2)];
            if (heightDiff >= 0)
                dy =  g_pRfraTbl[ heightDiff * g_Conf.depthRes + ((g_Conf.depthRes - 1) / 2 + waterData[x])];
            else
                dy = -g_pRfraTbl[-heightDiff * g_Conf.depthRes + ((g_Conf.depthRes - 1) / 2 + waterData[x])];
            
            // X軸方向のずれを範囲内に収まるように調整
            if (x + dx < 0)
                dx = -x;
            else if (x + dx > g_Conf.widthRes - 1)
                dx = g_Conf.widthRes - 1 - x;
            // Y軸方向のずれを範囲内に収まるように調整
            if (y + dy < 0)
                dy = -y;
            else if (y + dy > g_Conf.heightRes - 1)
                dy = g_Conf.heightRes - 1 - y;
            
            // ずれた位置の色を代入
            scrLine[x] = bgLine[x + dx + bgPitch * dy];
//            scrLine[x] = (waterData[x] + (g_Conf.depthRes - 1) / 2) * 255 / g_Conf.depthRes;
        }
        // 次の行にポインタを進める
        scrLine += scrPitch;
        bgLine += bgPitch;
        waterData += g_Conf.widthRes + 2;
    }

    // スクリーンサーフェスのロックを解除
    SDL_UnlockSurface(g_pBgImage);
    SDL_UnlockSurface(target);
}


