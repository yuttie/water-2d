
/*
 **************************************************************************
 *
 *  main.c - Water
 *
 *  Copyright  2004  Yuta Taniguchi
 **************************************************************************
*/


/* Log
 * <<< Version 0.2.1  2004/03/27 12:10:05 >>>
 * Fix:  波紋を静める処理の割算部分が切り上げになっていて、
 * 
 * <<< Version 0.2.0  2004/03/27 12:10:05 >>>
 * Src:  プログラムの設定を一つの構造体にまとめた。
 * Src:  main関数をすっきりさせた。
 * New:  フルスクリーンモードでの実行を指定する起動時オプションを追加した。
 * Fix:  屈折テーブルの使用するメモリ量を1/8で済むように修正。
 * New:  背景画像から幅と高さを決めていたのを止め、自由に指定できる起動時オプションを追加した。
 * New:  自動的に指定された背景画像のサイズを、ウィンドウのサイズにリサイズするようにした。
 * New:  ドラッグ時に行う補間の分割数を指定する起動時オプションを追加した。
 * Fix:  水面の高さの解像度を変えると、屈折がおかしくなるのを修正。
 * Fix:  波紋の大きさを変えると、屈折がおかしくなるのを修正。
 * New:  水面の高さの解像度を直接指定できる起動時オプションを追加した。
 * Mod:  オプションの新規追加に従ってヘルプを更新。
 * New:  キャッシュ作成時に進行状況をタイトルバーに表示するようにした。
 * Fix:  屈折テーブルの計算が低精度だったのを修正。
 *
 * <<< Version 0.1.3  2004/02/28 15:55:20 >>>
 * Src:  サーフェスの拡大縮小関数を実装した。但し、実装だけ。
 *
 * <<< Version 0.1.2  2004/02/27 14:28:17 >>>
 * New:  ヘルプを表示する起動時オプションを加えた。
 *
 * <<< Version 0.1.1  2004/02/27 08:25:23 >>>
 * New: ドラッグ時の波紋の発生を補間付きにした
 * New: qキーで終了できるようにした
 *
 * <<< Version 0.1.0  2004/02/27 08:04:06 >>>
 * Fix:  波紋の加算により水面が最高点で飽和し平坦化する現象を直した。
 *
 * <<< Version 0.0.0  ????/??/?? ??:??:?? >>>
 * Info: Windows + Delphiにより、Javaによる実装などを参考にして作成した。
 *       GDIを使い、MMXも使わず、非常に性能が悪かった。
 *       その後、Delphiのインラインアセンブラを利用して、主要計算部分をMMX化した。
 *       DelphiからC言語(Mingw)に変更するのを機に、SDLを利用することにした。
 *       波紋の形を予め計算しキャッシュ化しておくことで、高速化を行った。
 *       「屈折によるずれ」の計算をきちんと行い、キャッシュ化しておくことにした。
 *       フレネル反射を実装したが、上方から当たる白い光の反射を計算するだけなので、
 *           全く綺麗では無く、むしろ無い方が良いと判断したので、削除した。
*/


/* To Do
 9 完全に水面が落ち着かない(0にならない)
 9 DrawWater関数内で拡大して描画する案の実験
 9 vorboseモード
 5 変数名
 4 壁エディタ(円、直線、ピクセル単位の編集)
 3 SSE等のサポート
 2 画面に情報を表示(FPS等)
 1 他のディスプレイモードサポート
 */


/* Max FPS(400x300) - debug
   55.34
   55.39 - inline
*/


/* Idea
 * FPSの最大値を指定できるようにする
 * -vオプション指定で起動時にMaxWaterHeightなどのシミュレーションで使われる数値をコンソールに表示する
 * 背景の画像の対応形式を増やす
 * 
*/


/* 仕様
 * ARGB-32bitのみサポート
*/


/* インクルードファイル */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "SDL/SDL.h"
#include "imgscale.h"


/* 型 */
typedef Sint16 PosData;
struct ProgramOption
{
    char *bgImgFile;         // 背景イメージのファイル名
    int resolution;          // 縦方向の分解能
    int rippleRadius;        // 発生させる波紋の半径(イメージが指定されていない場合に使用)
    int rippleHeight;        // 発生させる波紋の高さ
    
    int waterWidth;          // 水面の幅
    int waterHeight;         // 水面の高さ
    int windowWidth;         // ウィンドウの幅
    int windowHeight;        // ウィンドウの高さ
    bool isFullScreen;       // フルスクリーンか否か
    
    int ipDivide;            // 波紋の補間(InterPolating)を何分割で行うか
};


/* 関数のプロトタイプ宣言 */
int main(int argc, char **argv);
void InitProc(int argc, char **argv);
inline bool EventProc();
void ExitProc();
bool ParseArgument(int argc, char **argv);
void InitSDL();
PosData *CreateRippleData();
Uint16 *CreateRefractionTable();
inline void CalcWater();
inline void RippleOut(int x, int y);
inline void DrawWater();


/* 定数 */
const double PI = 3.1415926535;    // 円周率


/* テキストデータ */
const char const *HELP_MESSAGE = "<usage>\n"
                                 "    water [option]\n"
                                 "<Options>\n"                                                          
                                 "    -h    : show this help.\n"
                                 "    -f    : use full-screen mode.\n"
                                 "    -rN   : N is number of ripple size.\n"
                                 "    -dN   : N is number of interpolating division.\n"
                                 "    -bF   : F is file name which will be used for background.\n"
                                 "    -sNxM : N is width and M is height. NxM means water size in simulation.\n"
                                 "    -mN   : N is number of memory size in KB which this program can use as cache.\n"
                                 "    -pN   : N is number of height resolution.\n"
                                 "<complement>\n"
                                 "    Memory-Size(-mN) = Height-Resolution(-pN)^2 + 2byte\n";


/* グローバル変数 */
struct ProgramOption progOption;
PosData *newWater;          // 水面データ
PosData *curWater;          // 水面データ
PosData *oldWater;          // 水面データ
PosData *rippleData;        // 波紋データ
Uint16 *refractionTable;    // 屈折による変移量テーブル
SDL_Surface *screen;
SDL_Surface *bg;


/* メイン関数 */
int main(int argc, char **argv)
{
    // プログラム初期化
    InitProc(argc, argv);
    
    // FPS計測用変数
    Uint32 startTime = 0;
    Uint32 endTime = 0;
    Uint32 frameCount = 0;
    
    // 無限ループ
    while(!EventProc())
    {
        // 水面を描画する
        DrawWater();

        // 表示を更新
        SDL_UpdateRect(screen, 0, 0, 0, 0);
        
        // 水面を入れ替える
        PosData *tmp = oldWater;
        oldWater = curWater;
        curWater = newWater;
        newWater = tmp;
        
        // 水面を計算
        CalcWater();
        
        // FPS計測
        endTime = SDL_GetTicks();
        frameCount++;
        if (endTime - startTime > 1000)
        {
            char cap[16];
            sprintf(cap, "FPS : %#.2f", frameCount * 1000.0 / (endTime - startTime));
            SDL_WM_SetCaption(cap, NULL);
            startTime = SDL_GetTicks();
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
    progOption.bgImgFile = "bgimage.bmp";
    progOption.resolution = 511;
    progOption.rippleRadius = 8;
    progOption.rippleHeight = (progOption.resolution - 1) / 2;
    progOption.waterWidth = 256;
    progOption.waterHeight = 192;
    progOption.windowWidth = progOption.waterWidth;
    progOption.windowHeight = progOption.waterHeight;
    progOption.isFullScreen = false;
    progOption.ipDivide = 1;
    
    // 引数解析
    if (ParseArgument(argc, argv))
    {
        // 各種情報を標準出力に表示する
        printf("ImageFileName : %s\n", progOption.bgImgFile);
        printf("HeightResolution : %d\n", progOption.resolution);
        printf("RippleRadius : %d\n", progOption.rippleRadius);
        printf("RippleHeight : %d\n", progOption.rippleHeight);
        printf("Width : %d\n", progOption.waterWidth);
        printf("Height : %d\n", progOption.waterHeight);
        printf("WindowWidth : %d\n", progOption.windowWidth);
        printf("WindowHeight : %d\n", progOption.windowHeight);
        printf("InterpolatingDivide : %d\n", progOption.ipDivide);
        if (progOption.isFullScreen)    puts("FullScreen : ON");
        else                            puts("FullScreen : OFF");
    }
    
    // SDL初期化してスクリーンサーフェスを取得
    InitSDL();
    
    // 背景画像読み込み
    SDL_Surface *tmpBg = SDL_LoadBMP(progOption.bgImgFile);
    if (tmpBg == NULL)
    {
        fprintf(stderr, "背景画像ファイルが読み込めません。\n");
        exit(EXIT_FAILURE);
    }
    
    // 背景サーフェスをスクリーンサーフェスの形式に変換
    tmpBg = SDL_ConvertSurface(tmpBg, screen->format, SDL_SWSURFACE);
    
    // 読み込んだ背景を水面の大きさに合わせる
    bg = SDL_CreateRGBSurface(SDL_SWSURFACE, progOption.waterWidth, progOption.waterHeight, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    ScaleCopySurface(SM_BI_LINEAR, tmpBg, bg);
    
    // 水面データ配列作成
    newWater = calloc((progOption.waterWidth + 2) * (progOption.waterHeight + 2), sizeof(PosData));
    curWater = calloc((progOption.waterWidth + 2) * (progOption.waterHeight + 2), sizeof(PosData));
    oldWater = calloc((progOption.waterWidth + 2) * (progOption.waterHeight + 2), sizeof(PosData));
    
    // 波紋データ作成
    rippleData = CreateRippleData();
    
    // 屈折テーブルを作成
    refractionTable = CreateRefractionTable();
    
    // 屈折テーブルアクセス時に必要なオフセットを加算しておく
//    refractionTable += (progOption.resolution * (MaxWaterHeight - MinWaterHeight + 1)) - MinWaterHeight;
}


/* イベント処理関数 */
inline bool EventProc()
{
    int radius = progOption.rippleRadius;
    
    bool exitFlag = false;    // 終了を求めるイベントが有るかどうかのフラグ
    static int preCursorX;    // 前のイベント発生時のマウスカーソルのX座標
    static int preCursorY;    // 前のイベント発生時のマウスカーソルのY座標
    SDL_Event event;          // イベントに関する情報が入る構造体(共用体?)
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
            {   // キーダウンイベント
                switch (event.key.keysym.sym)
                {
                    case SDLK_q:
                    {   // qキー
                        exitFlag = true;
                        break;
                    }
                    default:
                    {   // その他のキー(ハンドルされないキー)
                        break;
                    }
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {   // マウスボタンダウンイベント
                if (event.button.state == SDL_PRESSED)
                {
                    if ((event.button.x > radius) && (event.button.x < progOption.waterWidth - radius) &&
                        (event.button.y > radius) && (event.button.y < progOption.waterHeight - radius))
                    {
                        // 波紋を発生
                        RippleOut(event.button.x, event.button.y);
                        preCursorX = event.motion.x;
                        preCursorY = event.motion.y;
                    }
                }
                break;
            }
            case SDL_MOUSEMOTION:
            {   // マウスカーソル移動イベント
                if (event.motion.state == SDL_PRESSED)
                {
                    if ((event.motion.x > radius) && (event.motion.x < progOption.waterWidth - radius) &&
                        (event.motion.y > radius) && (event.motion.y < progOption.waterHeight - radius))
                    {
                        // 波紋を発生(補間する)
                        for (int i = 0; i < progOption.ipDivide; i++)
                        {
                            RippleOut((event.motion.x * i + preCursorX * (progOption.ipDivide - i)) / progOption.ipDivide,
                                      (event.motion.y * i + preCursorY * (progOption.ipDivide - i)) / progOption.ipDivide);
                        }
                        preCursorX = event.motion.x;
                        preCursorY = event.motion.y;
                    }
                }
                break;
            }
            case SDL_QUIT:
            {   // 終了イベント
                exitFlag = true;
                break;
            }
        }
    }
    return exitFlag;
}


/* プログラム終了前の後始末 */
void ExitProc()
{
    // 屈折テーブルアクセス時に必要なオフセットを減算しておく
//    refractionTable -= (-(MinWaterHeight - MaxWaterHeight) * (MaxWaterHeight - MinWaterHeight + 1)) - MinWaterHeight;
    
    // メモリの解放
    SDL_FreeSurface(bg);
    free(newWater);
    free(curWater);
    free(oldWater);
    free(rippleData);
    free(refractionTable);
    
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
                {   // 使い方を表示して終了
                    printf(HELP_MESSAGE); exit(EXIT_SUCCESS);
                    break;
                }
                case 'v':
                {   // 冗長な表示を行う
                    isVerboseMode = true;
                    break;
                }
                case 'f':
                {   // フルスクリーンモードに設定
                    progOption.isFullScreen = true;
                    break;
                }
                case 'r':
                {   // 波紋の大きさを取得
                    if (argv[i][2] == '\0') progOption.rippleRadius = atoi(&argv[++i][0]);
                    else                    progOption.rippleRadius = atoi(&argv[i][2]);
                    break;
                }
                case 'd':
                {   // 補間時の分割数を取得
                    if (argv[i][2] == '\0') progOption.ipDivide = atoi(&argv[++i][0]);
                    else                    progOption.ipDivide = atoi(&argv[i][2]);
                    break;
                }
                case 'b':
                {   // 背景のファイル名を取得
                    if (argv[i][2] == '\0') progOption.bgImgFile = &argv[++i][0];
                    else                    progOption.bgImgFile = &argv[i][2];
                    break;
                }
                case 's':
                {   // 計算上の水面のサイズを取得
                    if (argv[i][2] == '\0')
                    {
                        char *endPtr;
                        progOption.waterWidth = (int)strtoul(&argv[++i][0], &endPtr, 10);
                        progOption.waterHeight = (int)strtoul(++endPtr, NULL, 10);
                    }
                    else
                    {
                        char *endPtr;
                        progOption.waterWidth = (int)strtoul(&argv[i][2], &endPtr, 10);
                        progOption.waterHeight = (int)strtoul(++endPtr, NULL, 10);
                    }
                    progOption.windowWidth = progOption.waterWidth;
                    progOption.windowHeight = progOption.waterHeight;
                    break;
                }
                case 'm':
                {   // メモリ使用可能量(KB単位)を取得
                    // 水面の高さの精度(解像度)に影響する
                    int mem;
                    if (argv[i][2] == '\0') mem = atoi(&argv[++i][0]);
                    else                    mem = atoi(&argv[i][2]);
                    
                    // 水面の高さに換算し、補正
                    // 波の計算時に、上下左右を足すから
                    // (PosData(int16_t)の最大値32768)÷4=8192より
                    // 各ピクセルの高さの最大値は8191、最小値は-8191
                    // 屈折テーブル用メモリを確保量(メモリ使用量≒progOption.resolution^2 * 2byte(16bit))
                    progOption.resolution = (int)(sqrt(mem * 1024 / 2.0));
                    if (progOption.resolution > 8191 * 2 + 1)
                        progOption.resolution = 8192 * 2 +1;
                    
                    //
//                    printf("%d\n", progOption.resolution);
                    
                    // 波紋の高さを再計算
                    progOption.rippleHeight = (progOption.resolution - 1) / 2;
                    
                    break;
                }
                case 'p':
                {   // 水面の高さの精度(解像度)を取得
                    // メモリ使用量に影響する
                    if (argv[i][2] == '\0') progOption.resolution = atoi(&argv[++i][0]);
                    else                    progOption.resolution = atoi(&argv[i][2]);
                    
                    // 波紋の高さを再計算
                    progOption.rippleHeight = (progOption.resolution - 1) / 2;
                    
                    break;
                }
                default:
                {   // その他の場合
                    fprintf(stderr, "不正な引数が含まれています。\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    
    // 冗長な表示を行うか否かを返す
    return isVerboseMode;
}


/* CPUID命令の有無確認関数 */
/*
bool CheckCPUID()
{
    bool b;
    asm volatile (
        "pushfd\n"
        "popl %%eax\n"
        "movl %%eax, %%ebx\n"
        "xorl $0x200000, %%eax\n"
        "pushl %%eax\n"
        "popfd\n"
        "pushfd\n"
        "popl %%eax\n"
        "cmpl %%eax, %%ebx\n"
        ":same\n"
        : "=r" (b)
        );
}
*/

/* SDL初期化関数 */
void InitSDL()
{
    // SDLを初期化
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        // エラーメッセージを表示
        fprintf(stderr, "初期化に失敗しました：%s\n", SDL_GetError());
        
        // プログラムを終了
        exit(EXIT_FAILURE);
    }
    
    // ディスプレイデバイスの初期化フラグを設定
    Uint32 screenFlags = SDL_SWSURFACE;
    if (progOption.isFullScreen) screenFlags |= SDL_FULLSCREEN;
    
    // ディスプレイデバイスを初期化
    screen = SDL_SetVideoMode(progOption.windowWidth, progOption.windowHeight, 32, screenFlags);
    
    // 初期化に失敗したら、エラーメッセージを表示して終了
    if (screen == NULL)
    {
        // エラーメッセージを表示
        fprintf(stderr, "ビデオの初期化に失敗しました：%s\n", SDL_GetError());
        
        // SDL終了
        SDL_Quit();
        
        // プログラムを終了
        exit(EXIT_FAILURE);
    }
}


/* 波紋データ作成関数 */
PosData *CreateRippleData()
{
    int radius = progOption.rippleRadius;
    // 波紋データ作成
    PosData *rippleData = calloc((2 * radius + 1) * (2 * radius + 1), sizeof(PosData));
    PosData *pData = rippleData;
    
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
                *pData = (PosData)((cos(t) + 1) * progOption.rippleHeight / 2);
            }
            else
            {   // 円外
                *pData = 0;
            }
            pData++;
        }
    }
    return rippleData;
}


/* 屈折テーブル作成関数 */
Uint16 *CreateRefractionTable()
{
    // 縦方向の分解能がp倍になると、波紋の高さもp倍になる
    // ここでピクセルの幅をp倍にしないと、p倍縦長の波紋になってしまう。
    // それでは困るので、ピクセルの幅をp倍する。
    // ここまでで、仮想的にp倍のスケールで計算した事になる。
    // そこで、算出したずれを1/p倍する。
    const double p = progOption.resolution / 512.0;
    
    // 波紋の半径がq倍になると、波紋の高さはそのままなので、
    // q倍横長の波紋になってしまう。
    // そこで、波紋の高さをq倍する。
    // このまま計算すると、q倍のスケールで計算した事になる。
    // よって、算出したずれを1/q倍する。
    const double q = progOption.rippleRadius / 8.0;
    
    const double PIXEL_WIDTH = 64.0 * p;         // ピクセルの幅
    const double REFRACTION_INDEX = 1.33;    // 水の絶対屈折率

    // テーブル用メモリを確保(メモリ使用量≒progOption.resolution^2 * 2byte(16bit))
    // ずれは、水面の高さと、そのピクセルの左右または上下の高低差に影響される
    // Left < Right
    Uint16 *refractionTable = calloc(progOption.resolution * progOption.resolution, sizeof(Uint16));
    
    // 計算開始
    int i = 0, d = progOption.resolution / 10;
    for (int heightDiff = 0; heightDiff < progOption.resolution; heightDiff++)
    {
        // 入射角を求める
        double t1 = atan(q * heightDiff / (2 * PIXEL_WIDTH));
        
        // 屈折角を求める
        double t2 = asin(sin(t1) / REFRACTION_INDEX);
        
        // 各水面の高さに応じたずれの量を求める
        // ずれは、水面の高さと、そのピクセルの左右または上下の高低差に影響される
        for (int waterHeight = 0; waterHeight < progOption.resolution; waterHeight++)
        {
            int index = progOption.resolution * heightDiff + waterHeight;
            double delta = q * waterHeight * tan(t1 - t2);
            refractionTable[index] = (Uint16)(delta / p / q);
        }
        if (i < heightDiff)
        {
            char cap[32];
            sprintf(cap, "Creating Cache : %#.2f%%...", 100.0 * heightDiff / progOption.resolution);
            SDL_WM_SetCaption(cap, NULL);
            i += d;
        }
    }
    
    // テーブルへのポインタを返す
    return refractionTable;
}


/* 水面計算関数 */
inline void CalcWater()
{
    // 飽和処理用上限/下限配列
    int maxHeight = (progOption.resolution - 1) / 2;
    PosData upperLimit[4] = { maxHeight,  maxHeight,  maxHeight,  maxHeight};
    PosData lowerLimit[4] = {-maxHeight, -maxHeight, -maxHeight, -maxHeight};
    Sint16 addnum[4] = {63, 63, 63, 63};
    Sint16 mulnum[4] = {32256, 32256, 32256, 32256};
    
    // 初期位置×に移動
    // 壁壁壁壁壁
    // 壁×水水水
    // 壁水水水水
    // 壁水水水水
    int SurfaceWidth = progOption.waterWidth;
    int SurfaceHeight = progOption.waterHeight;
    PosData *tmp = newWater;
    PosData *newWater = tmp;
    tmp = curWater;
    PosData *curWater = tmp;
    tmp = oldWater;
    PosData *oldWater = tmp;
    newWater += 1 * (SurfaceWidth + 2) + 1;
    curWater += 1 * (SurfaceWidth + 2) + 1;
    oldWater += 1 * (SurfaceWidth + 2) + 1;

    for (int y = 0; y < SurfaceHeight; y++)
    {
        for (int x = 0; x < SurfaceWidth; x += 4)
        {
            asm volatile (
                // ■□■□ここから計算処理□■□■
                // curWaterの上下左右を加算
                "movq -2(%%esi), %%mm0\n"                            // curWaterの左を読み込む
                "paddsw 2(%%esi), %%mm0\n"                           // curWaterの右を加算
                "paddsw (%%esi, %%ebx, 2), %%mm0\n"                  // curWaterの下を加算
                "neg %%ebx\n" "paddsw (%%esi, %%ebx, 2), %%mm0\n"    // curWaterの上を加算
                
                // 加算したものを2分の1にする（0方向に丸める版）
                "movq %%mm0, %%mm1\n"      // コピー
                "psrlw $15, %%mm1\n"       // 最上位ビットを抽出
                "paddsw %%mm1, %%mm0\n"    // 最上位ビットを加算
                "psraw $1, %%mm0\n"        // 算術右シフト演算
                
                // oldWaterの中央を減算
                "psubsw (%%eax), %%mm0\n"
                
                // ■□■□ここから飽和処理□■□■
                // 上限越えチェック用配列を読み込む
                "movq (%%ecx), %%mm5\n"
                "movq %%mm0, %%mm6\n"       // 比較で上書きされるのでコピー
                
                // 上限を越えている箇所を探す（マスク作成）
                "pcmpgtw %%mm5, %%mm0\n"    // 上限を越えている所が1、他は0になる
                
                // 飽和させる 
                "pand %%mm0, %%mm5\n"       // 配列を上限越え箇所のみにする
                "pandn %%mm6, %%mm0\n"      // 上限越え箇所を0にする
                "por %%mm5, %%mm0\n"        // 論理和合成
                
                // 下限越えチェック用配列を読み込む
                "movq (%%edx), %%mm5\n"
                "movq %%mm5, %%mm6\n"       // 比較で上書きされるのでコピー
                
                // 下限を越えている箇所を探す（マスク作成）
                "pcmpgtw %%mm0, %%mm6\n"    // 下限を越えている所が1になる
                
                // 飽和させる
                "pand %%mm6, %%mm5\n"       // 配列を下限越え箇所のみにする
                "pandn %%mm0, %%mm6\n"      // 下限越え箇所を0にする
                "por %%mm5, %%mm6\n"        // 論理和合成
                
                // メモリに書き戻す
                "movq %%mm6, (%%edi)\n"
            :
            : "S" (curWater),              // 今の水面
              "D" (newWater),              // 新しい水面
              "a" (oldWater),              // 古い水面
              "b" (SurfaceWidth + 2),      // 水面データの幅
              "c" (upperLimit),            // オーバーチェック用配列
              "d" (lowerLimit)             // アンダーチェック用配列
            : "memory");
#ifdef MUL
            // 76.08
            asm volatile (
                // ■□■□ここから波を静める処理□■□■
                // p/256倍にする。pを掛けて、上位8ビットのみ取得。
                // 下位8ビットを右シフトしたのと同じ
                "movq (%%edi), %%mm0\n"
                "movq (%%esi), %%mm1\n"
                "movq %%mm1, %%mm2\n"
                "pmulhw %%mm0, %%mm1\n"
                "pmullw %%mm0, %%mm2\n"
                "psllw $1, %%mm1\n"
                "psrlw $15, %%mm2\n"
                "por %%mm2, %%mm1\n"
                "movq %%mm1, (%%edi)\n"    // メモリに書き戻す
            :
            : "S" (mulnum),
              "D" (newWater)
            : "memory");
#else
            // 75.66
            asm volatile (
                // ■□■□ここから波を静める処理□■□■
                // X分のX-1倍にする(X分の1を引く)
                "movq (%%edi), %%mm0\n"    // コピー(X分のX-1になる)
                "movq %%mm0, %%mm1\n"      // コピー(X分の1になる)
                "movq %%mm0, %%mm2\n"      // 最上位ビット抽出用
                "movq (%%esi), %%mm3\n"    // 63
                "psrlw $15, %%mm2\n"       // 最上位ビットを抽出
                "paddsw %%mm2, %%mm1\n"    // 最上位ビットを加算
                "paddsw %%mm3, %%mm1\n"    // Xで割る前に、(X-1)を足しておく(小数点以下を切り上げるため)
                "psraw $6, %%mm1\n"        // 算術右シフト演算(X分の1倍にする)
                "psubsw %%mm1, %%mm0\n"    // X分の1倍を引いてX分のX-1倍にする
                "movq %%mm0, (%%edi)\n"    // メモリに書き戻す
            :
            : "S" (addnum),
              "D" (newWater)
            : "memory");
#endif
            // ポインタを進める
            newWater += 4;
            curWater += 4;
            oldWater += 4;
        }
        // ポインタを進める
        newWater += 2;
        curWater += 2;
        oldWater += 2;
    }

    // MMX終了
    asm ("emms\n");
}


/* 波紋発生関数 */
inline void RippleOut(int x, int y)
{
    int SurfaceWidth = progOption.waterWidth;
//    int SurfaceHeight = progOption.waterHeight;
    int radius = progOption.rippleRadius;
    
    // 波データの左上隅にあたる位置に移動
    int incValue = (SurfaceWidth + 2) * ((y + 1) - radius) + ((x + 1) - radius);
    PosData *tmp = newWater;
    PosData *newWater = tmp;
    tmp = curWater;
    PosData *curWater = tmp;
    newWater += incValue;
    curWater += incValue;
    tmp = rippleData;
    PosData *rippleData = tmp;
    
    // 波紋を発生させる
    incValue = (SurfaceWidth + 2) - (2 * radius + 1);
    for (int iy = -radius; iy <= radius; iy++)
    {
        for (int ix = -radius; ix <= radius; ix++)
        {
            if (*rippleData != 0)
            {
                // 異符号なら、普通に足す
                if (((*newWater >= 0) && (*rippleData < 0)) || ((*newWater <= 0) && (*rippleData > 0)))
                    *newWater += *rippleData;
                // 同符号で波紋の高さより低いなら、そこまで引き上げる
                else if (*newWater < *rippleData)
                    *newWater = *rippleData;
                // 同符号で波紋の高さより大きいか同じであるなら、何もしない
                else
                    ;
                
                // 異符号なら、普通に足す
                if (((*curWater >= 0) && (*rippleData < 0)) || ((*curWater <= 0) && (*rippleData > 0)))
                    *curWater += *rippleData;
                // 同符号で波紋の高さより低いなら、そこまで引き上げる
                else if (*curWater < *rippleData)
                    *curWater = *rippleData;
                // 同符号で波紋の高さより大きいか同じであるなら、何もしない
                else
                    ;
            }
            
            // ポインタを進める
            rippleData++;
            newWater++;
            curWater++;
        }
        // 次に書きこむ行の頭に移動
        newWater += incValue;
        curWater += incValue;
    }
}


/* 水面描画関数 */
inline void DrawWater()
{
    int SurfaceWidth = progOption.waterWidth;
    int SurfaceHeight = progOption.waterHeight;
    int Resolution = progOption.resolution;
    
    // スクリーンサーフェスをロック
    SDL_LockSurface(screen);
    SDL_LockSurface(bg);
    
    // 最初のピクセルへ移動
    PosData *waterData = newWater;
    waterData += (SurfaceWidth + 2) + 1;
    
    // 最初の行へのポインタで初期化
    Uint32 *scrLine = (Uint32 *)screen->pixels;
    Uint32 *bgLine = (Uint32 *)bg->pixels;
    
    // ピッチを取得
    Uint32 scrPitch = screen->pitch / sizeof(Uint32);
    Uint32 bgPitch = bg->pitch / sizeof(Uint32);
    
    // 描画
    for (int y = 0; y < SurfaceHeight; y++)
    {
        for (int x = 0; x < SurfaceWidth; x++)
        {
            int heightDiff;
            int dx;
            int dy;
            heightDiff = waterData[x + 1] - waterData[x - 1];
            if (heightDiff >= 0)    dx =  refractionTable[ heightDiff * Resolution + ((Resolution - 1) / 2 + waterData[x])];
            else                    dx = -refractionTable[-heightDiff * Resolution + ((Resolution - 1) / 2 + waterData[x])];
            heightDiff = waterData[x + (SurfaceWidth + 2)] - waterData[x - (SurfaceWidth + 2)];
            if (heightDiff >= 0)    dy =  refractionTable[ heightDiff * Resolution + ((Resolution - 1) / 2 + waterData[x])];
            else                    dy = -refractionTable[-heightDiff * Resolution + ((Resolution - 1) / 2 + waterData[x])];
            
            // 範囲内に収まるように調整
            if (x + dx < 0)                         dx = -x;
            else if (x + dx > SurfaceWidth - 1)     dx = SurfaceWidth - 1 - x;
            if (y + dy < 0)                         dy = -y;
            else if (y + dy > SurfaceHeight - 1)    dy = SurfaceHeight - 1 - y;
            
            // ずれた位置の色を代入
            scrLine[x] = bgLine[x + bgPitch * dy + dx];
//            scrLine[x] = (waterData[x] + (Resolution - 1) / 2) * 255 / Resolution;
        }
        // 次の行にポインタを進める
        scrLine += scrPitch;
        bgLine += bgPitch;
        waterData += SurfaceWidth + 2;
    }

    // スクリーンサーフェスのロックを解除
    SDL_UnlockSurface(bg);
    SDL_UnlockSurface(screen);
}


