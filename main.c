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
 * ** �����٤κ����͡�
 * ** ��ե�������󥰤򤹤롣
 * 70 �Ȥ�ȿ�ͤ�ͳüȿ�ͤˤ��롣
 * 70 ���ޥơ��֥륭��å����ե����뤫���ɤ߹�����ͤˤ��롣
 * 70 PaintToSurface�ؿ���ǳ��礷�����褹��Ƥμ¸�
 * 60 vorbose�⡼��
 * 50 ����å����Ȥ�ʤ��⡼��
 * 40 �ɥ��ǥ���(�ߡ�ľ�����ԥ�����ñ�̤��Խ�)
 * 30 SSE����¾�Υޥ����ǥ���̿��Υ��ݡ���
 * 30 CPU�ޥ����ǥ���̿��Υ��ݡ��Ⱦ����ˤ�äƽ��������ؤ���
 * 20 ���̤˾����ɽ��(FPS��)
 * 10 ¾�Υǥ����ץ쥤�⡼�ɥ��ݡ���
 */


/* <<< Max FPS(400x300) - debug >>>
 */


/* <<< Idea >>>
 * FPS�κ����ͤ����Ǥ���褦�ˤ���
 * -v���ץ�������ǵ�ư����MaxWaterHeight�ʤɤΥ��ߥ�졼�����ǻȤ�����ͤ򥳥󥽡����ɽ������
 * �طʤβ������б����������䤹
 * 
 */


/* <<< Specification >>>
 * ARGB-32bit�Τߥ��ݡ���
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
const double PI = 3.1415926535;    // �߼�Ψ

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
Config   gConfig;         // �ץ���������
float   *gPosition;       // ����
float   *gVelocity;       // ®��
float   *gForce;          // ®��
float   *gRippleGeometry; // ���ߤ�����ǡ����ؤΥݥ���
Uint16  *gRefractionTbl;  // ���ޤˤ���Ѱ��̥ơ��֥�
SDL_Surface *gScreen;     // �����꡼�󥵡��ե���
SDL_Surface *gImage;      // �طʥ��᡼�������ե���
void (*stepFunc)(float dt);       // ���̥ǡ��������ؿ�


/* Main Function */
int main(int argc, char **argv) {
    // �ץ��������
    initProc(argc, argv);
    // FPS��¬���ѿ�
    Uint32 startTick = SDL_GetTicks();
    Uint32 endTick = 0;
    Uint32 frameCount = 0;
    // ̵�¥롼��
    while(!eventProc()) {
        // ���̤�׻�
        stepFunc(0.1);
        // ���̤򥹥��꡼�󥵡��ե��������褹��
        paintToSurface(gScreen);
        // ɽ���򹹿�
        SDL_UpdateRect(gScreen, 0, 0, 0, 0);
        // FPS��¬
        endTick = SDL_GetTicks();
        frameCount++;
        if (endTick - startTick > 1000) {
            // ������ɥ�����ץ�����ɽ�����뤿���ʸ��������
            char cap[16];
            sprintf(cap, "FPS : %#.2f", frameCount * 1000.0 / (endTick - startTick));
            // ʸ����򥦥���ɥ�����ץ���������
            SDL_WM_SetCaption(cap, NULL);
            // �������Ȼ���ե졼����Υꥻ�å�
            startTick = SDL_GetTicks();
            frameCount = 0;
        }
    }
    // �ץ���ཪλ����
    exitProc();
    return 0;
}


/* �ץ���������ؿ� */
void initProc(int argc, char **argv) {
    // �ץ����������ǥե�����ͤǽ����
    gConfig.imagePath = "bgimage.bmp";
    //gConfig.meshWidth = 300;  �����Ǥ����ꤷ�ʤ�
    //gConfig.meshHeight = 240; �����Ǥ����ꤷ�ʤ�
    gConfig.virtualWidth = 300;
    gConfig.virtualHeight = 240;
    gConfig.windowWidth = 300;
    gConfig.windowHeight = 240;
    gConfig.rippleRadius = 4;
    gConfig.attRate = 0.99;
    gConfig.scale = 1.0;
    gConfig.isFullScreen = false;
    gConfig.isVerbose = false;
    
    // ��������
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
    
    // ��Ĺ�⡼�ɻ� : �����ͤ�ɸ����Ϥ˽��Ϥ���
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
        // SIMD̿��Υ����å�
        printf("CPUID: %s\n", CheckCPUID() ? "Supported" : "Not Supported");
        printf("SSE: %s\n", CheckSSE() ? "Supported" : "Not Supported");
        printf("SSE2: %s\n", CheckSSE2() ? "Supported" : "Not Supported");
        printf("EDX: %d\n", GetEDX());
    }

    // CPUID������̥ǡ����򹹿�����ؿ������
    stepFunc = stepFPU;

    // SDL��������ƥ����꡼�󥵡��ե��������
    initSDL();
    
    // �طʲ����ɤ߹���
    SDL_Surface *tmpBg = SDL_LoadBMP(gConfig.imagePath);
    if (tmpBg == NULL) {
        // ���顼ɽ���򤷤ƽ�λ
        fprintf(stderr, MSG_ERROR_LOAD_IMAGE, gConfig.imagePath);
        exit(EXIT_FAILURE);
    }
    
    // �طʥ����ե����򥹥��꡼�󥵡��ե�����Ʊ���������Ѵ�
    tmpBg = SDL_ConvertSurface(tmpBg, gScreen->format, SDL_SWSURFACE);
    
    // �ɤ߹�����طʤ򥦥���ɥ����礭���˹�碌��
    gImage = SDL_CreateRGBSurface(SDL_SWSURFACE,
            gConfig.windowWidth, gConfig.windowHeight, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    scaleCopySurface(SM_BI_LINEAR, tmpBg, gImage);
    
    // ��å���Υ����������(����4���ܿ���·����)
    gConfig.meshWidth = 4 * (((gConfig.virtualWidth + 2) + 3) / 4);
    gConfig.meshHeight = gConfig.virtualHeight + 2;

    // ���̥ǡ����������
    int meshSize =  gConfig.meshWidth * gConfig.meshHeight;
    gPosition = calloc(meshSize, sizeof(float));
    gVelocity = calloc(meshSize, sizeof(float));
    gForce    = calloc(meshSize, sizeof(float));
    
    // ����ǡ�������
    gRippleGeometry = createRippleData(gConfig.rippleRadius);
    
    // ���ޥơ��֥�����
    gRefractionTbl = createRefractionTable();
    
    // ���ޥơ��֥륢����������ɬ�פʥ��ե��åȤ�û����Ƥ���
//    g_pRfraTbl += (gConfig.depthRes * (MaxWaterHeight - MinWaterHeight + 1)) - MinWaterHeight;
}


/* ���٥�Ƚ����ؿ� */
bool eventProc() {
    int topLimit = gConfig.rippleRadius;
    int bottomLimit = gConfig.windowHeight - gConfig.rippleRadius;
    int leftLimit = gConfig.rippleRadius;
    int rightLimit = gConfig.windowWidth - gConfig.rippleRadius;
    bool exitFlag = false;    // ��λ�׵᤬ͭ�뤫�ݤ�
    SDL_Event event;          // ���٥�Ȥ˴ؤ���������빽¤��(������?)
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN: // ���������󥤥٥��
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: // ��λ����(ESC, Q)
            case SDLK_q:
                exitFlag = true;
                break;
            case SDLK_KP_PLUS: // +����
            case SDLK_PLUS:
                gConfig.rippleRadius += 4;
                free(gRippleGeometry);
                gRippleGeometry = createRippleData(gConfig.rippleRadius);
                break;
            case SDLK_KP_MINUS: // -����
            case SDLK_MINUS:
                gConfig.rippleRadius -= 4;
                if (gConfig.rippleRadius < 0) {
                    gConfig.rippleRadius = 0;
                }
                free(gRippleGeometry);
                gRippleGeometry = createRippleData(gConfig.rippleRadius);
                break;
            default: // ����¾�Υ���(�ϥ�ɥ뤵��ʤ�����)
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN: // �ޥ����ܥ�������󥤥٥��
            // �ޥ����Υܥ��󤬲�����Ƥ����
            if (event.button.state == SDL_PRESSED) {
                // ����ȯ����ǽ�ΰ�ˤ��뤫�ɤ���������å�
                if (leftLimit < event.button.x && event.button.x < rightLimit &&
                    topLimit < event.button.y && event.button.y < bottomLimit) {
                    // �����ȯ��
                    rippleOut(event.button.x, event.button.y);
                }
            }
            break;
        case SDL_MOUSEMOTION: // �ޥ������������ư���٥��
            // �ޥ����Υܥ��󤬲�����Ƥ����
            if (event.motion.state == SDL_PRESSED) {
                // ����ȯ����ǽ�ΰ�ˤ��뤫�ɤ���������å�
                if (leftLimit < event.motion.x && event.motion.x < rightLimit &&
                    topLimit < event.motion.y && event.motion.y < bottomLimit) {
                    // �����ȯ��
                    rippleOut(event.motion.x, event.motion.y);
                }
            }
            break;
        case SDL_QUIT: // ��λ���٥��
            exitFlag = true;
            break;
        }
    }
    return exitFlag;
}


/* �ץ���ཪλ���θ���� */
void exitProc() {
    // ���ޥơ��֥륢����������ɬ�פʥ��ե��åȤ򸺻����Ƥ���
//    g_pRfraTbl -= (-(MinWaterHeight - MaxWaterHeight) * (MaxWaterHeight - MinWaterHeight + 1)) - MinWaterHeight;
    
    // ����β���
    SDL_FreeSurface(gImage);
    free(gPosition);
    free(gVelocity);
    free(gForce);
    free(gRippleGeometry);
    free(gRefractionTbl);
    
    // SDL��λ
    SDL_Quit();
}

/* SDL������ؿ� */
void initSDL() {
    // SDL������
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        // ���顼ɽ���򤷤ƽ�λ
        fprintf(stderr, MSG_ERROR_INIT_SDL, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    // �ǥ����ץ쥤�ǥХ����ν�����ե饰������
    Uint32 screenFlags = SDL_SWSURFACE;
    if (gConfig.isFullScreen) {
        screenFlags |= SDL_FULLSCREEN;
    }
    
    // �ǥ����ץ쥤�ǥХ���������
    gScreen = SDL_SetVideoMode(gConfig.windowWidth, gConfig.windowHeight,
                                 32, screenFlags);
    
    // ������˼��Ԥ����顢���顼��å�������ɽ�����ƽ�λ
    if (gScreen == NULL) {
        // ���顼ɽ���򤷤ƽ�λ
        fprintf(stderr, MSG_ERROR_INIT_VIDEO, SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

/* ����ǡ��������ؿ� */
float *createRippleData(int radius) {
    // ����ǡ�������
    float *rippleData = calloc((radius + 1 + radius) * (radius + 1 + radius), sizeof(float));
    float *pData = rippleData;
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            // x = 0, y = 0���濴�Ȥ���
            // �濴����ε�Υ���������
            int r2 = (x * x) + (y * y);
            if (r2 < radius * radius) {
                // ����
                double t = PI * (sqrt(r2) / radius);
                *pData = (float)(100 * (cos(t) + 1));
            }
            pData++;
        }
    }
    return rippleData;
}


/* ���ޥơ��֥�����ؿ� */
Uint16 *createRefractionTable() {
/*
    // 512��ǥե���Ȥ�ʬ��ǽ�Ȥ���ȡ�
    // ��������ʬ��ǽ��p�ܤˤʤ�ȡ�����ι⤵��p�ܤˤʤ�
    // �����ǥԥ����������p�ܤˤ��ʤ��ȡ�p�ܽ�Ĺ������ˤʤäƤ��ޤ���
    // ����ǤϺ���Τǡ��ԥ����������p�ܤ��롣
    // �����ޤǤǡ�����Ū��p�ܤΥ�������Ƿ׻��������ˤʤ롣
    // �����ǡ����Ф��������1/p�ܤ��롣
    const double p = gConfig.depthRes / 512.0;
    
    // 8��ǥե���Ȥ�Ⱦ�¤Ȥ���ȡ�
    // �����Ⱦ�¤�q�ܤˤʤ�ȡ�����ι⤵�Ϥ��ΤޤޤʤΤǡ�
    // q�ܲ�Ĺ������ˤʤäƤ��ޤ���
    // �����ǡ�����ι⤵��q�ܤ��롣
    // ���Τޤ޷׻�����ȡ�q�ܤΥ�������Ƿ׻��������ˤʤ롣
    // ��äơ����Ф��������1/q�ܤ��롣
    const double q = gConfig.rippleRadius / 8.0;
*/    
    const double PIXEL_WIDTH = 1.0;     // �ԥ��������
    const double REFRACTION_INDEX = 1.33;    // ������ж���Ψ

    // �ơ��֥��ѥ�������(��������̢�gConfig.depthRes^2 * 2byte(16bit))
    // ����ϡ����̤ι⤵�ȡ����Υԥ�����κ����ޤ��Ͼ岼�ι��㺹�˱ƶ������
    // Left < Right
    Uint16 *refractionTable = calloc(10000, sizeof(Uint16));
    
    // �׻�����
    int i = 0, d = 100 / 10;
    for (int heightDiff = 0; heightDiff < 100; heightDiff++) {
        // ���ͳѤ����
        double t1 = atan((heightDiff * 100.0 / (100 / 2.0)) / (2.0 * PIXEL_WIDTH));
        
        // ���޳Ѥ����
        double t2 = asin(sin(t1) / REFRACTION_INDEX);
        
        // �ƿ��̤ι⤵�˱�����������̤����
        // ����ϡ����̤ι⤵�ȡ����Υԥ�����κ����ޤ��Ͼ岼�ι��㺹�˱ƶ������
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
    
    // �ơ��֥�ؤΥݥ��󥿤��֤�
    return refractionTable;
}

/* ����ȯ���ؿ� */
void rippleOut(int x, int y) {
    float *position = gPosition;
    float *velocity = gVelocity;
    // �ȥǡ����κ�����ˤ�������֤˰�ư
    int incValue = gConfig.meshWidth * ((y + 1) - gConfig.rippleRadius) +
                                       ((x + 1) - gConfig.rippleRadius);
    position += incValue;
    velocity += incValue;
    //float *riplData = gRippleGeometry;
    
    // �����ȯ��������
    incValue = gConfig.meshWidth - (2 * gConfig.rippleRadius + 1);
    for (int y = -gConfig.rippleRadius; y <= gConfig.rippleRadius; y++) {
        for (int x = -gConfig.rippleRadius; x <= gConfig.rippleRadius; x++) {
            double t = sqrt(x * x + y * y);
            if (t < gConfig.rippleRadius) {
                *position += 1 * (cos(PI * t / gConfig.rippleRadius) + 1.0);
            }
            // �ݥ��󥿤�ʤ��
            position++;
            velocity++;
        }
        // ���˽񤭤���Ԥ�Ƭ�˰�ư
        position += incValue;
        velocity += incValue;
    }
}


/* ��������ؿ� */
void paintToSurface(SDL_Surface *target) {
    // �����꡼�󥵡��ե������å�
    SDL_LockSurface(target);
    SDL_LockSurface(gImage);
    // �ǽ�Υԥ�����ذ�ư
    float *waterData = gPosition;
    waterData += gConfig.meshWidth + 1;
    // �ǽ�ιԤؤΥݥ��󥿤ǽ����
    Uint32 *scrLine = (Uint32 *)target->pixels;
    Uint32 *bgLine = (Uint32 *)gImage->pixels;
    // �ԥå������
    Uint32 scrPitch = target->pitch / sizeof(Uint32);
    Uint32 bgPitch = gImage->pitch / sizeof(Uint32);
    // ����(x,y�ϥ����꡼�󥵡��ե�����ΰ���)
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
            // Ŭ���ϰϤˤʤ�褦�˽���
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
        // ���ιԤ˥ݥ��󥿤�ʤ��
        scrLine += scrPitch;
        bgLine += bgPitch;
        waterData += gConfig.meshWidth;
    }

    // �����꡼�󥵡��ե����Υ�å�����
    SDL_UnlockSurface(gImage);
    SDL_UnlockSurface(target);
}


