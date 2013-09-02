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
    char  *pBgImgPath;      // �طʥ��᡼���Υե�����̾
    int    depthRes;        // �����β�����
    int    riplRadius;      // ȯ�������������Ⱦ��
    double riplDepth;       // ȯ������������κ��翼��
    int    widthRes;        // ���ߥ�졼�Ȥ�����̤����β�����
    int    heightRes;       // ���ߥ�졼�Ȥ�����̤ι⤵�β�����
    int    wndWidth;        // ������ɥ�����
    int    wndHeight;       // ������ɥ��ι⤵
    double attRate;         // ����Ψ
    double scale;
    int    csrIPDiv;        // �����������֤�ʬ��ǹԤ���
    bool   isFullScreen;    // �ե륹���꡼���ݤ�
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

