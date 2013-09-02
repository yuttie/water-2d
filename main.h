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
    char  *imagePath;      // �طʥ��᡼���Υե�����̾
    int    meshWidth;      // ��å���(�ºݤΥǡ�����)����
    int    meshHeight;     // ��å���(�ºݤΥǡ�����)�ι⤵
    int    virtualWidth;   // �ͤ��Ƥ��벾�ۿ��̤���
    int    virtualHeight;  // �ͤ��Ƥ��벾�ۿ��̤ι⤵
    int    windowWidth;    // ɽ������(������ɥ�)����
    int    windowHeight;   // ɽ������(������ɥ�)�ι⤵
    int    rippleRadius;   // ȯ�������������Ⱦ��
    float  attRate;        // ����Ψ
    float  scale;          // ����Ψ(������ɥ�������/���ۥ�����)
    bool   isFullScreen;   // �ե륹���꡼���ݤ�
    bool   isVerbose;      // �ܺ٤���Ϥ��뤫
} Config;

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
extern const char const *MSG_ERROR_INVALID_OPTION;
extern const char const *MSG_ERROR_LOAD_IMAGE;
extern const char const *MSG_ERROR_INIT_SDL;
extern const char const *MSG_ERROR_INIT_VIDEO;
extern const char const *MSG_HELP;

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

