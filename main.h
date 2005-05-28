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
#include "SDL/SDL.h"

/* Type Definition */
typedef Sint16 PosData;
typedef struct __ProgConfig
{
    char  *pBgImgPath;      // 背景イメージのファイル名
    int    depthRes;        // 深さの解像度
    int    riplRadius;      // 発生させる波紋の半径
    double riplDepth;       // 発生させる波紋の最大深さ
    int    widthRes;        // シミュレートする水面の幅の解像度
    int    heightRes;       // シミュレートする水面の高さの解像度
    int    wndWidth;        // ウィンドウの幅
    int    wndHeight;       // ウィンドウの高さ
    double attRate;         // 減衰率
    double scale;
    int    csrIPDiv;        // カーソルの補間を何分割で行うか
    bool   isFullScreen;    // フルスクリーンか否か
} ProgConfig;


/* Function Prototype Declaration */
int      main(int argc, char **argv);
void         InitProc(int argc, char **argv);
bool             ParseArgument(int argc, char **argv);
void             InitSDL();
PosData         *CreateRippleData();
Uint16          *CreateRefractionTable();
bool         EventProc();
void             RippleOut(int x, int y);
void         PaintToSurface(SDL_Surface *target);
void         ExitProc();

#endif

