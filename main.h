/*
 *******************************************************************************
 *
 *  main.h - Water-2d
 *
 *  Copyright  2004  Yuta Taniguchi
 *******************************************************************************
 */


#ifndef __MAIN_H
#define __MAIN_H

/* Include Files */
#include <stdbool.h>
#include "SDL/SDL.h"

/* Type Definition */
typedef struct __Config {
    char  *imagePath;      // 背景イメージのファイル名
    int    meshWidth;      // メッシュ(実際のデータ列)の幅
    int    meshHeight;     // メッシュ(実際のデータ列)の高さ
    int    virtualWidth;   // 考えている仮想水面の幅
    int    virtualHeight;  // 考えている仮想水面の高さ
    int    windowWidth;    // 表示する(ウィンドウ)の幅
    int    windowHeight;   // 表示する(ウィンドウ)の高さ
    int    rippleRadius;   // 発生させる波紋の半径
    float  attRate;        // 減衰率
    float  scale;          // 拡大率(ウィンドウサイズ/仮想サイズ)
    bool   isFullScreen;   // フルスクリーンか否か
    bool   isVerbose;      // 詳細を出力するか
} Config;

/* Constant */
extern const int hMargin;
extern const int vMargin;

/* Global Variable */
extern Config   gConfig;
extern float   *gPosition;
extern float   *gVelocity;
extern float   *gForce;
extern float   *gRippleGeometry;
extern Uint16  *gRefractionTbl;
extern SDL_Surface *gScreen;
extern SDL_Surface *gImage;
extern void (*stepFunc)(float dt);

/* TextData */
extern const char * const MSG_ERROR_INVALID_OPTION;
extern const char * const MSG_ERROR_LOAD_IMAGE;
extern const char * const MSG_ERROR_INIT_SDL;
extern const char * const MSG_ERROR_INIT_VIDEO;
extern const char * const MSG_HELP;

/* Function Prototype Declaration */
int main(int argc, char **argv);
void initProc(int argc, char **argv);
bool eventProc();
void exitProc();
void initSDL();
float *createRippleData(int radius);
Uint16 *createRefractionTable();
void rippleOut(int x, int y);
void paintToSurface(SDL_Surface *target);
#endif

