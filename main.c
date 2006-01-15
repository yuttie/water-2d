/*
 *******************************************************************************
 *
 *  main.c - MeshWater-2D-Float
 *
 *  Copyright  2004-2006  Yuta Taniguchi
 *******************************************************************************
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
#include "option_parser.h"
#include "main.h"

/* Constant */
const double PI = 3.1415926535;    // 円周率

/* TextData */
const char const *MSG_ERROR_INVALID_OPTION =
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
Config   gConfig;         // プログラムの設定
float   *gPosition;       // 位置
float   *gVelocity;       // 速度
float   *gForce;          // 速度
float   *gRippleGeometry; // 現在の波紋データへのポインタ
Uint16  *gRefractionTbl;  // 屈折による変移量テーブル
SDL_Surface *gScreen;     // スクリーンサーフェス
SDL_Surface *gImage;      // 背景イメージサーフェス
void (*stepFunc)(float dt);       // 水面データ更新関数


/* Main Function */
int main(int argc, char **argv) {
    // プログラム初期化
    initProc(argc, argv);
    // FPS計測用変数
    Uint32 startTick = SDL_GetTicks();
    Uint32 endTick = 0;
    Uint32 frameCount = 0;
    // 無限ループ
    while(!eventProc()) {
        // 水面を計算
        stepFunc(0.1);
        // 水面をスクリーンサーフェスに描画する
        paintToSurface(gScreen);
        // 表示を更新
        SDL_UpdateRect(gScreen, 0, 0, 0, 0);
        // FPS計測
        endTick = SDL_GetTicks();
        frameCount++;
        if (endTick - startTick > 1000) {
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
    exitProc();
    return 0;
}


/* プログラム初期化関数 */
void initProc(int argc, char **argv) {
    // プログラムの設定をデフォルト値で初期化
    gConfig.imagePath = "bgimage.bmp";
    //gConfig.meshWidth = 300;  ここでは設定しない
    //gConfig.meshHeight = 240; ここでは設定しない
    gConfig.virtualWidth = 300;
    gConfig.virtualHeight = 240;
    gConfig.windowWidth = 300;
    gConfig.windowHeight = 240;
    gConfig.rippleRadius = 4;
    gConfig.attRate = 0.99;
    gConfig.scale = 1.0;
    gConfig.isFullScreen = false;
    gConfig.isVerbose = false;
    
    // 引数解析
    switch(parseOption(&gConfig, argc, argv)) {
    case OPTPSR_SUCCESS:
        break;
    case OPTPSR_SUCCESS_EXIT:
        exit(EXIT_SUCCESS);
        break;
    case OPTPSR_FAILED:
        exit(EXIT_FAILURE);
        break;
    }
    
    // 冗長モード時 : 設定値を標準出力に出力する
    if (gConfig.isVerbose) {
        printf("gConfig.imagePath: %s", gConfig.imagePath);
        printf("gConfig.meshWidth: %d", gConfig.meshWidth);
        printf("gConfig.meshHeight: %d", gConfig.meshHeight);
        printf("gConfig.virtualWidth: %d", gConfig.virtualWidth);
        printf("gConfig.virtualHeight: %d", gConfig.virtualHeight);
        printf("gConfig.windowWidth: %d", gConfig.windowWidth);
        printf("gConfig.windowHeight: %d", gConfig.windowHeight);
        printf("gConfig.rippleRadius: %f", (double)gConfig.rippleRadius);
        printf("gConfig.attRate: %f", gConfig.attRate);
        printf("gConfig.scale: %f", gConfig.scale);
        //printf("gConfig.isFullScreen", gConfig.isFullScreen);
        //printf("gConfig.isVerbose", gConfig.isVerbose);
        // SIMD命令のチェック
        printf("CPUID: %s\n", CheckCPUID() ? "Supported" : "Not Supported");
        printf("SSE: %s\n", CheckSSE() ? "Supported" : "Not Supported");
        printf("SSE2: %s\n", CheckSSE2() ? "Supported" : "Not Supported");
        printf("EDX: %d\n", GetEDX());
    }

    // CPUIDから水面データを更新する関数を決定
    stepFunc = stepFPU;

    // SDL初期化してスクリーンサーフェスを取得
    initSDL();
    
    // 背景画像読み込み
    SDL_Surface *tmpBg = SDL_LoadBMP(gConfig.imagePath);
    if (tmpBg == NULL) {
        // エラー表示をして終了
        fprintf(stderr, MSG_ERROR_LOAD_IMAGE, gConfig.imagePath);
        exit(EXIT_FAILURE);
    }
    
    // 背景サーフェスをスクリーンサーフェスと同じ形式に変換
    tmpBg = SDL_ConvertSurface(tmpBg, gScreen->format, SDL_SWSURFACE);
    
    // 読み込んだ背景をウィンドウの大きさに合わせる
    gImage = SDL_CreateRGBSurface(SDL_SWSURFACE,
            gConfig.windowWidth, gConfig.windowHeight, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    scaleCopySurface(SM_BI_LINEAR, tmpBg, gImage);
    
    // メッシュのサイズを決定(幅は4の倍数に揃える)
    gConfig.meshWidth = 4 * (((gConfig.virtualWidth + 2) + 3) / 4);
    gConfig.meshHeight = gConfig.virtualHeight + 2;

    // 水面データ配列作成
    int meshSize =  gConfig.meshWidth * gConfig.meshHeight;
    gPosition = calloc(meshSize, sizeof(float));
    gVelocity = calloc(meshSize, sizeof(float));
    gForce    = calloc(meshSize, sizeof(float));
    
    // 波紋データ作成
    gRippleGeometry = createRippleData(gConfig.rippleRadius);
    
    // 屈折テーブルを作成
    gRefractionTbl = createRefractionTable();
    
    // 屈折テーブルアクセス時に必要なオフセットを加算しておく
//    g_pRfraTbl += (gConfig.depthRes * (MaxWaterHeight - MinWaterHeight + 1)) - MinWaterHeight;
}


/* イベント処理関数 */
bool eventProc() {
    int topLimit = gConfig.rippleRadius;
    int bottomLimit = gConfig.windowHeight - gConfig.rippleRadius;
    int leftLimit = gConfig.rippleRadius;
    int rightLimit = gConfig.windowWidth - gConfig.rippleRadius;
    bool exitFlag = false;    // 終了要求が有るか否か
    SDL_Event event;          // イベントに関する情報が入る構造体(共用体?)
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN: // キーダウンイベント
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: // 終了キー(ESC, Q)
            case SDLK_q:
                exitFlag = true;
                break;
            case SDLK_KP_PLUS: // +キー
            case SDLK_PLUS:
                gConfig.rippleRadius += 4;
                free(gRippleGeometry);
                gRippleGeometry = createRippleData(gConfig.rippleRadius);
                break;
            case SDLK_KP_MINUS: // -キー
            case SDLK_MINUS:
                gConfig.rippleRadius -= 4;
                if (gConfig.rippleRadius < 0) {
                    gConfig.rippleRadius = 0;
                }
                free(gRippleGeometry);
                gRippleGeometry = createRippleData(gConfig.rippleRadius);
                break;
            default: // その他のキー(ハンドルされないキー)
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN: // マウスボタンダウンイベント
            // マウスのボタンが押されている時
            if (event.button.state == SDL_PRESSED) {
                // 波紋発生可能領域にいるかどうかをチェック
                if (leftLimit < event.button.x && event.button.x < rightLimit &&
                    topLimit < event.button.y && event.button.y < bottomLimit) {
                    // 波紋を発生
                    rippleOut(event.button.x, event.button.y);
                }
            }
            break;
        case SDL_MOUSEMOTION: // マウスカーソル移動イベント
            // マウスのボタンが押されている時
            if (event.motion.state == SDL_PRESSED) {
                // 波紋発生可能領域にいるかどうかをチェック
                if (leftLimit < event.motion.x && event.motion.x < rightLimit &&
                    topLimit < event.motion.y && event.motion.y < bottomLimit) {
                    // 波紋を発生
                    rippleOut(event.motion.x, event.motion.y);
                }
            }
            break;
        case SDL_QUIT: // 終了イベント
            exitFlag = true;
            break;
        }
    }
    return exitFlag;
}


/* プログラム終了前の後始末 */
void exitProc() {
    // 屈折テーブルアクセス時に必要なオフセットを減算しておく
//    g_pRfraTbl -= (-(MinWaterHeight - MaxWaterHeight) * (MaxWaterHeight - MinWaterHeight + 1)) - MinWaterHeight;
    
    // メモリの解放
    SDL_FreeSurface(gImage);
    free(gPosition);
    free(gVelocity);
    free(gForce);
    free(gRippleGeometry);
    free(gRefractionTbl);
    
    // SDL終了
    SDL_Quit();
}

/* SDL初期化関数 */
void initSDL() {
    // SDLを初期化
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        // エラー表示をして終了
        fprintf(stderr, MSG_ERROR_INIT_SDL, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    // ディスプレイデバイスの初期化フラグを設定
    Uint32 screenFlags = SDL_SWSURFACE;
    if (gConfig.isFullScreen) {
        screenFlags |= SDL_FULLSCREEN;
    }
    
    // ディスプレイデバイスを初期化
    gScreen = SDL_SetVideoMode(gConfig.windowWidth, gConfig.windowHeight,
                                 32, screenFlags);
    
    // 初期化に失敗したら、エラーメッセージを表示して終了
    if (gScreen == NULL) {
        // エラー表示をして終了
        fprintf(stderr, MSG_ERROR_INIT_VIDEO, SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

/* 波紋データ作成関数 */
float *createRippleData(int radius) {
    // 波紋データ作成
    float *rippleData = calloc((radius + 1 + radius) * (radius + 1 + radius), sizeof(float));
    float *pData = rippleData;
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            // x = 0, y = 0を中心とする
            // 中心からの距離の二乗を求める
            int r2 = (x * x) + (y * y);
            if (r2 < radius * radius) {
                // 円内
                double t = PI * (sqrt(r2) / radius);
                *pData = (float)(100 * (cos(t) + 1));
            }
            pData++;
        }
    }
    return rippleData;
}


/* 屈折テーブル作成関数 */
Uint16 *createRefractionTable() {
/*
    // 512をデフォルトの分解能とすると、
    // 縦方向の分解能がp倍になると、波紋の高さもp倍になる
    // ここでピクセルの幅をp倍にしないと、p倍縦長の波紋になってしまう。
    // それでは困るので、ピクセルの幅をp倍する。
    // ここまでで、仮想的にp倍のスケールで計算した事になる。
    // そこで、算出したずれを1/p倍する。
    const double p = gConfig.depthRes / 512.0;
    
    // 8をデフォルトの半径とすると、
    // 波紋の半径がq倍になると、波紋の高さはそのままなので、
    // q倍横長の波紋になってしまう。
    // そこで、波紋の高さをq倍する。
    // このまま計算すると、q倍のスケールで計算した事になる。
    // よって、算出したずれを1/q倍する。
    const double q = gConfig.rippleRadius / 8.0;
*/    
    const double PIXEL_WIDTH = 1.0;     // ピクセルの幅
    const double REFRACTION_INDEX = 1.33;    // 水の絶対屈折率

    // テーブル用メモリを確保(メモリ使用量≒gConfig.depthRes^2 * 2byte(16bit))
    // ずれは、水面の高さと、そのピクセルの左右または上下の高低差に影響される
    // Left < Right
    Uint16 *refractionTable = calloc(10000, sizeof(Uint16));
    
    // 計算開始
    int i = 0, d = 100 / 10;
    for (int heightDiff = 0; heightDiff < 100; heightDiff++) {
        // 入射角を求める
        double t1 = atan((heightDiff * 100.0 / (100 / 2.0)) / (2.0 * PIXEL_WIDTH));
        
        // 屈折角を求める
        double t2 = asin(sin(t1) / REFRACTION_INDEX);
        
        // 各水面の高さに応じたずれの量を求める
        // ずれは、水面の高さと、そのピクセルの左右または上下の高低差に影響される
        for (int height = 0; height < 100; height++) {
            int index = 100 * heightDiff + height;
            double delta = (height * 100.0 / (100 / 2.0)) * tan(t1 - t2);
            refractionTable[index] = (Uint16)(delta);
        }
        if (i < heightDiff) {
            char cap[32];
            sprintf(cap, "Creating Cache : %#.2f%%...", 100.0 * heightDiff / 100);
            SDL_WM_SetCaption(cap, NULL);
            i += d;
        }
    }
    
    // テーブルへのポインタを返す
    return refractionTable;
}

/* 波紋発生関数 */
void rippleOut(int x, int y) {
    float *position = gPosition;
    float *velocity = gVelocity;
    // 波データの左上隅にあたる位置に移動
    int incValue = gConfig.meshWidth * ((y + 1) - gConfig.rippleRadius) +
                                       ((x + 1) - gConfig.rippleRadius);
    position += incValue;
    velocity += incValue;
    //float *riplData = gRippleGeometry;
    
    // 波紋を発生させる
    incValue = gConfig.meshWidth - (2 * gConfig.rippleRadius + 1);
    for (int y = -gConfig.rippleRadius; y <= gConfig.rippleRadius; y++) {
        for (int x = -gConfig.rippleRadius; x <= gConfig.rippleRadius; x++) {
            double t = sqrt(x * x + y * y);
            if (t < gConfig.rippleRadius) {
                *position += 1 * (cos(PI * t / gConfig.rippleRadius) + 1.0);
            }
            // ポインタを進める
            position++;
            velocity++;
        }
        // 次に書きこむ行の頭に移動
        position += incValue;
        velocity += incValue;
    }
}


/* 水面描画関数 */
void paintToSurface(SDL_Surface *target) {
    // スクリーンサーフェスをロック
    SDL_LockSurface(target);
    SDL_LockSurface(gImage);
    // 最初のピクセルへ移動
    float *waterData = gPosition;
    waterData += gConfig.meshWidth + 1;
    // 最初の行へのポインタで初期化
    Uint32 *scrLine = (Uint32 *)target->pixels;
    Uint32 *bgLine = (Uint32 *)gImage->pixels;
    // ピッチを取得
    Uint32 scrPitch = target->pitch / sizeof(Uint32);
    Uint32 bgPitch = gImage->pitch / sizeof(Uint32);
    // 描画(x,yはスクリーンサーフェス上の位置)
    for (int y = 0; y < gConfig.windowHeight; y++) {
        for (int x = 0; x < gConfig.windowWidth; x++) {
#if 0
            const double pw = 1.0;
            const double ri = 1.33;
            int dhx = waterData[x + 1] - waterData[x - 1];
            int dx;
            if (dhx >= 0) {
                dx = waterData[x] * tan(atan(dhx / (2 * pw)) - asin(sin(atan(dhx / (2 * pw))) / ri));
            } else {
                dx = -waterData[x] * tan(atan(-dhx / (2 * pw)) - asin(sin(atan(-dhx / (2 * pw))) / ri));
            }
            int dhy = waterData[x + gConfig.meshWidth] - waterData[x - gConfig.meshWidth];
            int dy;
            if (dhy >= 0) {
                dy = waterData[y] * tan(atan(dhy / (2 * pw)) - asin(sin(atan(dhy / (2 * pw))) / ri));
            } else {
                dy = -waterData[y] * tan(atan(-dhy / (2 * pw)) - asin(sin(atan(-dhy / (2 * pw))) / ri));
            }
            // 適正範囲になるように修正
            if (x + dx < 0) {
                dx = -x;
            } else if (x + dx >= gConfig.windowWidth) {
                dx = gConfig.windowWidth - x - 1;
            }
            if (y + dy < 0) {
                dy = -y;
            } else if (y + dy >= gConfig.windowWidth) {
                dy = gConfig.windowHeight - y - 1;
            }
            dx *= 10.0; dy *= 10.0;
            scrLine[x] = bgLine[x + bgPitch * dy + dx];
#else
            int c = 255 * (waterData[x] + 4) / 8;
            scrLine[x] = c << 16 | c << 8 | c;
#endif
        }
        // 次の行にポインタを進める
        scrLine += scrPitch;
        bgLine += bgPitch;
        waterData += gConfig.meshWidth;
    }

    // スクリーンサーフェスのロックを解除
    SDL_UnlockSurface(gImage);
    SDL_UnlockSurface(target);
}


