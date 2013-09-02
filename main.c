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
 * New:  ������礭����+-������ưŪ���Ѥ��뤳�Ȥ������褦�ˤ�����
 * 
 * >>> 2004/07/22 00:08:50 <<<
 * Src:  �׻���ʬ�Υ��르�ꥺ���n/32768�ܥ��르�ꥺ������򤷡�(2^n-1)/2^n�ܥ��르�ꥺ�����������
 * New:  ����Ψ���ѹ����뵯ư�����ץ������ɲä�����
 * 
 * >>> 2004/07/21 16:03:00 <<<
 * Fix:  ���̤��Ť��黻�γ任��ʬ���ڼΤƤ�Ԥ��褦���ѹ������������Ȥ��Ťޤ�褦�˽���������
 * Src:  ����������ͭ���å�������const��������ư����ǤޤȤ����������ͤˤ�����
 * New:  ESC�����Ǥ⽪λ�Ǥ���褦�ˤ�����
 * Src:  CPUID��MMX��SSE��SSE2̿���¸�ߥ����å��ؿ������������
 * 
 * <<< 2004/03/27 12:10:05 >>>
 * Src:  �ץ�����������Ĥι�¤�ΤˤޤȤ᤿��
 * Src:  main�ؿ��򤹤ä��ꤵ������
 * New:  �ե륹���꡼��⡼�ɤǤμ¹Ԥ���ꤹ�뵯ư�����ץ������ɲä�����
 * Fix:  ���ޥơ��֥�λ��Ѥ�������̤�1/8�ǺѤ�褦�˽�����
 * New:  �طʲ����������ȹ⤵����Ƥ����Τ�ߤᡢ��ͳ���礭�������Ǥ��뵯ư�����ץ������ɲä�����
 * New:  ��ưŪ�˻��ꤵ�줿�طʲ����Υ������򡢥�����ɥ��Υ������˥ꥵ��������褦�ˤ�����
 * New:  �ɥ�å����˹Ԥ���֤�ʬ�������ꤹ�뵯ư�����ץ������ɲä�����
 * Fix:  ���̤ι⤵�β����٤��Ѥ���ȡ����ޤ����������ʤ�Τ�����
 * Fix:  ������礭�����Ѥ���ȡ����ޤ����������ʤ�Τ�����
 * New:  ���̤ι⤵�β����٤�ľ�ܻ���Ǥ��뵯ư�����ץ������ɲä�����
 * Mod:  ���ץ����ο����ɲä˽��äƥإ�פ򹹿���
 * New:  ����å���������˿ʹԾ����򥿥��ȥ�С���ɽ������褦�ˤ�����
 * Fix:  ���ޥơ��֥�η׻��������٤��ä��Τ�����
 *
 * <<< 2004/02/28 15:55:20 >>>
 * Src:  �����ե����γ���̾��ؿ������������â��������������
 *
 * <<< 2004/02/27 14:28:17 >>>
 * New:  �إ�פ�ɽ�����뵯ư�����ץ�����ä�����
 *
 * <<< 2004/02/27 08:25:23 >>>
 * New: �ɥ�å����������ȯ��������դ��ˤ���
 * New: q�����ǽ�λ�Ǥ���褦�ˤ���
 *
 * <<< 2004/02/27 08:04:06 >>>
 * Fix:  ����βû��ˤ����̤��ǹ�����˰�¤�ʿó�����븽�ݤ�ľ������
 *
 * <<< ????/??/?? ??:??:?? >>>
 * Info: Windows + Delphi�ˤ�ꡢJava�ˤ������ʤɤ򻲹ͤˤ��ƺ���������
 * Info: GDI��Ȥ���MMX��Ȥ鷺��������ǽ�������ä���
 * Info: ���θ塢Delphi�Υ���饤�󥢥���֥�����Ѥ��ơ����׷׻���ʬ��MMX��������
 * Info: Delphi����C����(Mingw)���ѹ�����Τ򵡤ˡ�SDL�����Ѥ��뤳�Ȥˤ�����
 * Info: ����η���ͽ��׻�������å��岽���Ƥ������Ȥǡ���®����Ԥä���
 * Info: �ֶ��ޤˤ�뤺��פη׻��򤭤���ȹԤ�������å��岽���Ƥ������Ȥˤ�����
 * Info: �ե�ͥ�ȿ�ͤ���������������������������򤤸���ȿ�ͤ�׻���������ʤΤǡ�
 *       ����ۤ����Ǥ�̵�����ष��̵�������ɤ���Ƚ�Ǥ����Τǡ����������
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
 * 55.34
 * 55.39 - inline
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
#include "main.h"


/* Constant */
const double PI = 3.1415926535;    // �߼�Ψ
// �׻����Թ������κ�����65536��1/4�������ѤǤ��ʤ�
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
ProgConfig   g_Conf;         // �ץ���������
PosData     *g_pNextData;    // ���ο��̥ǡ���
PosData     *g_pCrntData;    // ���ο��̥ǡ���
PosData     *g_pPrevData;    // ���ο��̥ǡ���
PosData     *g_pRiplData;    // ���ߤ�����ǡ����ؤΥݥ���
Uint16      *g_pRfraTbl;     // ���ޤˤ���Ѱ��̥ơ��֥�
SDL_Surface *g_pScreen;      // �����꡼�󥵡��ե���
SDL_Surface *g_pBgImage;     // �طʥ����ե���
void (*pCalcFunc)();         // ���̥ǡ��������ؿ�


/* Main Function */
int main(int argc, char **argv)
{
    // �ץ��������
    InitProc(argc, argv);
    
    // FPS��¬���ѿ�
    Uint32 startTick = SDL_GetTicks();
    Uint32 endTick = 0;
    Uint32 frameCount = 0;
    
    // ̵�¥롼��
    while(!EventProc())
    {
        // ���̤򥹥��꡼�󥵡��ե��������褹��
        PaintToSurface(g_pScreen);

        // ɽ���򹹿�
        SDL_UpdateRect(g_pScreen, 0, 0, 0, 0);
        
        // ���̥ǡ����������ؤ���
        PosData *tmp = g_pPrevData;
        g_pPrevData = g_pCrntData;
        g_pCrntData = g_pNextData;
        g_pNextData = tmp;
        
        // ���̤�׻�
        pCalcFunc();
        
        // FPS��¬
        endTick = SDL_GetTicks();
        frameCount++;
        if (endTick - startTick > 1000)
        {
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
    ExitProc();
    
    return 0;
}


/* �ץ���������ؿ� */
void InitProc(int argc, char **argv)
{
    // �ץ���४�ץ����Υǥե�����ͤ�����
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
    
    // ��������
    if (ParseArgument(argc, argv))
    {
        // ��Ĺ�⡼�ɻ� : �����ͤ�ɸ����Ϥ˽��Ϥ���
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
    // CPUID������̥ǡ����򹹿�����ؿ������
    pCalcFunc = CalculateMMX;

    // SDL��������ƥ����꡼�󥵡��ե��������
    InitSDL();
    
    // �طʲ����ɤ߹���
    SDL_Surface *tmpBg = SDL_LoadBMP(g_Conf.pBgImgPath);
    if (tmpBg == NULL)
    {
        // ���顼ɽ���򤷤ƽ�λ
        fprintf(stderr, MSG_ERROR_LOAD_IMAGE, g_Conf.pBgImgPath);
        exit(EXIT_FAILURE);
    }
    
    // �طʥ����ե����򥹥��꡼�󥵡��ե�����Ʊ���������Ѵ�
    tmpBg = SDL_ConvertSurface(tmpBg, g_pScreen->format, SDL_SWSURFACE);
    
    // �ɤ߹�����طʤ���̤��礭���˹�碌��
    g_pBgImage = SDL_CreateRGBSurface(SDL_SWSURFACE,
            g_Conf.widthRes, g_Conf.heightRes, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    ScaleCopySurface(SM_BI_LINEAR, tmpBg, g_pBgImage);
    
    // ���̥ǡ����������
    int arraySize = (1 + g_Conf.widthRes + 1) * (1 + g_Conf.heightRes + 1);
    g_pNextData = calloc(arraySize, sizeof(PosData));
    g_pCrntData = calloc(arraySize, sizeof(PosData));
    g_pPrevData = calloc(arraySize, sizeof(PosData));
    
    // ����ǡ�������
    g_pRiplData = CreateRippleData();
    
    // ���ޥơ��֥�����
    g_pRfraTbl = CreateRefractionTable();
    
    // ���ޥơ��֥륢����������ɬ�פʥ��ե��åȤ�û����Ƥ���
//    g_pRfraTbl += (g_Conf.depthRes * (MaxWaterHeight - MinWaterHeight + 1)) - MinWaterHeight;
}


/* ���٥�Ƚ����ؿ� */
bool EventProc()
{
    int topLimit = g_Conf.riplRadius;
    int bottomLimit = g_Conf.heightRes - g_Conf.riplRadius;
    int leftLimit = g_Conf.riplRadius;
    int rightLimit = g_Conf.widthRes - g_Conf.riplRadius;
    bool exitFlag = false;    // ��λ�׵᤬ͭ�뤫�ݤ�
    static int preCsrX;    // ���Υ��٥��ȯ�����Υޥ������������X��ɸ
    static int preCsrY;    // ���Υ��٥��ȯ�����Υޥ������������Y��ɸ
    SDL_Event event;          // ���٥�Ȥ˴ؤ���������빽¤��(������?)
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            // ���������󥤥٥��
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
            case SDLK_q:
                // ��λ����(ESC, Q)
                exitFlag = true;
                break;
            case SDLK_KP_PLUS:
            case SDLK_PLUS:
                // +����
                g_Conf.riplRadius += 4;
                if (g_Conf.riplRadius < 0) g_Conf.riplRadius = 0;
                free(g_pRiplData);
                g_pRiplData = CreateRippleData();
                break;
            case SDLK_KP_MINUS:
            case SDLK_MINUS:
                // -����
                g_Conf.riplRadius -= 4;
                if (g_Conf.riplRadius < 0) g_Conf.riplRadius = 0;
                free(g_pRiplData);
                g_pRiplData = CreateRippleData();
                break;
            default:
                // ����¾�Υ���(�ϥ�ɥ뤵��ʤ�����)
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            // �ޥ����ܥ�������󥤥٥��
            // �ޥ����Υܥ��󤬲�����Ƥ����
            if (event.button.state == SDL_PRESSED)
            {
                // ����ȯ����ǽ�ΰ�ˤ��뤫�ɤ���������å�
                if ((leftLimit < event.button.x) && (event.button.x < rightLimit) &&
                    (topLimit < event.button.y) && (event.button.y < bottomLimit))
                {
                    // �����ȯ��
                    RippleOut(event.button.x, event.button.y);
                    preCsrX = event.motion.x;
                    preCsrY = event.motion.y;
                }
            }
            break;
        case SDL_MOUSEMOTION:
            // �ޥ������������ư���٥��
            // �ޥ����Υܥ��󤬲�����Ƥ����
            if (event.motion.state == SDL_PRESSED)
            {
                // ����ȯ����ǽ�ΰ�ˤ��뤫�ɤ���������å�
                if ((leftLimit < event.motion.x) && (event.motion.x < rightLimit) &&
                    (topLimit < event.motion.y) && (event.motion.y < bottomLimit))
                {    // �����ȯ��

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
            // ��λ���٥��
            exitFlag = true;
            break;
        }
    }
    return exitFlag;
}


/* �ץ���ཪλ���θ���� */
void ExitProc()
{
    // ���ޥơ��֥륢����������ɬ�פʥ��ե��åȤ򸺻����Ƥ���
//    g_pRfraTbl -= (-(MinWaterHeight - MaxWaterHeight) * (MaxWaterHeight - MinWaterHeight + 1)) - MinWaterHeight;
    
    // ����β���
    SDL_FreeSurface(g_pBgImage);
    free(g_pNextData);
    free(g_pCrntData);
    free(g_pPrevData);
    free(g_pRiplData);
    free(g_pRfraTbl);
    
    // SDL��λ
    SDL_Quit();
}


/* ��ư���������ϴؿ� */
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
                // �Ȥ�����ɽ�����ƽ�λ
                printf(MSG_HELP); exit(EXIT_SUCCESS);
                break;
            case 'v':
                // ��Ĺ��ɽ����Ԥ�
                isVerboseMode = true;
                break;
            case 'f':
                // �ե륹���꡼��⡼�ɤ�����
                g_Conf.isFullScreen = true;
                break;
            case 'a':
                // ����θ���Ψ�����
                if (argv[i][2] == '\0') g_Conf.attRate = atof(&argv[++i][0]);
                else                    g_Conf.attRate = atof(&argv[i][2]);
                break;
            case 'b':
                // ���ܤ�����κ���ο���
                if (argv[i][2] == '\0') g_Conf.riplDepth = atof(&argv[++i][0]);
                else                    g_Conf.riplDepth = atof(&argv[i][2]);
                break;
            case 'r':
                // ������礭�������
                if (argv[i][2] == '\0') g_Conf.riplRadius = atoi(&argv[++i][0]);
                else                    g_Conf.riplRadius = atoi(&argv[i][2]);
                break;
            case 'd':
                // ��ֻ���ʬ��������
                if (argv[i][2] == '\0') g_Conf.csrIPDiv = atoi(&argv[++i][0]);
                else                    g_Conf.csrIPDiv = atoi(&argv[i][2]);
                break;
            case 'i':
                // �طʤΥե�����̾�����
                if (argv[i][2] == '\0') g_Conf.pBgImgPath = &argv[++i][0];
                else                    g_Conf.pBgImgPath = &argv[i][2];
                break;
            case 's':
                // �׻���ο��̤Υ����������
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
                // ������Ѳ�ǽ��(KBñ��)�����
                // ���̤ι⤵������(������)�˱ƶ�����
                int mem;
                if (argv[i][2] == '\0') mem = atoi(&argv[++i][0]);
                else                    mem = atoi(&argv[i][2]);
                
                // ���̤ι⤵�˴�����������
                // �Ȥη׻����ˡ��岼������­������
                // PosData(Sint16)�κ�����32767/4=8191���
                // �ƥԥ�����ι⤵�κ����ͤ�8191���Ǿ��ͤ�-8192
                // �����٤κ����ͤ�8191 - (-8192) + 1=16384
                // ���㺹�κǾ��ͤ�0�������ͤ�8191-(-8192)=16383
                // �Ĥޤꡢ����ϲ�����-1����������
                // ���ޥơ��֥��ѥ���γ����̤�
                // ������=g_Conf.depthRes^2 * 2byte(16bit))
                g_Conf.depthRes = (int)(sqrt(mem * 1024 / 2.0));

                // ���ꤵ�줿�����٤������ͤ�ۤ��Ƥ����齤�����롣
                if (g_Conf.depthRes > MAX_RESOLUTION)
                    g_Conf.depthRes = MAX_RESOLUTION;
                
                //
//                    printf("%d\n", g_Conf.depthRes);
                
                // ����ι⤵��Ʒ׻�
                //g_Conf.riplDepth = (g_Conf.depthRes - 1) / 2;
                
                break;
            }
            case 'p':
                // ���̤ι⤵������(������)�����
                // ��������̤˱ƶ�����
                if (argv[i][2] == '\0') g_Conf.depthRes = atoi(&argv[++i][0]);
                else                    g_Conf.depthRes = atoi(&argv[i][2]);
                
                // ���ꤵ�줿�����٤������ͤ�ۤ��Ƥ����齤�����롣
                if (g_Conf.depthRes > MAX_RESOLUTION)
                    g_Conf.depthRes = MAX_RESOLUTION;
                
                // ����ι⤵��Ʒ׻�
                //g_Conf.riplDepth = (g_Conf.depthRes - 1) / 2;
                
                break;
            default:
                // �����ʰ��������ꤵ�줿��硢���顼ɽ���򤷤ƽ�λ
                fprintf(stderr, MSG_ERROR_OPTION, argv[i]);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // �����ʰ��������ꤵ�줿��硢���顼ɽ���򤷤ƽ�λ
            fprintf(stderr, MSG_ERROR_OPTION, argv[i]);
            exit(EXIT_FAILURE);
        }
    }
    
    // ��Ĺ��ɽ����Ԥ����ݤ����֤�
    return isVerboseMode;
}


/* SDL������ؿ� */
void InitSDL()
{
    // SDL������
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        // ���顼ɽ���򤷤ƽ�λ
        fprintf(stderr, MSG_ERROR_INIT_SDL, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    // �ǥ����ץ쥤�ǥХ����ν�����ե饰������
    Uint32 screenFlags = SDL_SWSURFACE;
    if (g_Conf.isFullScreen) screenFlags |= SDL_FULLSCREEN;
    
    // �ǥ����ץ쥤�ǥХ���������
    g_pScreen = SDL_SetVideoMode(g_Conf.wndWidth, g_Conf.wndHeight, 32, screenFlags);
    
    // ������˼��Ԥ����顢���顼��å�������ɽ�����ƽ�λ
    if (g_pScreen == NULL)
    {
        // ���顼ɽ���򤷤ƽ�λ
        fprintf(stderr, MSG_ERROR_INIT_VIDEO, SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}


/* ����ǡ��������ؿ� */
PosData *CreateRippleData()
{
    int radius = g_Conf.riplRadius;
    // ����ǡ�������
    PosData *riplData = calloc((2 * radius + 1) * (2 * radius + 1), sizeof(PosData));
    PosData *pData = riplData;
    
    for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            // x = 0, y = 0���濴�Ȥ���
            // �濴����ε�Υ���������
            int r2 = (x * x) + (y * y);
            
            if (r2 < radius * radius)
            {   // ����
                double t = PI * (sqrt(r2) / radius);
                *pData = (PosData)((cos(t) + 1) * (g_Conf.depthRes / 2) * (g_Conf.riplDepth / 100.0) * (g_Conf.riplRadius / 8.0) / 2);
            }
            else
            {   // �߳�
                *pData = 0;
            }
            pData++;
        }
    }
    return riplData;
}


/* ���ޥơ��֥�����ؿ� */
Uint16 *CreateRefractionTable()
{
/*
    // 512��ǥե���Ȥ�ʬ��ǽ�Ȥ���ȡ�
    // ��������ʬ��ǽ��p�ܤˤʤ�ȡ�����ι⤵��p�ܤˤʤ�
    // �����ǥԥ����������p�ܤˤ��ʤ��ȡ�p�ܽ�Ĺ������ˤʤäƤ��ޤ���
    // ����ǤϺ���Τǡ��ԥ����������p�ܤ��롣
    // �����ޤǤǡ�����Ū��p�ܤΥ�������Ƿ׻��������ˤʤ롣
    // �����ǡ����Ф��������1/p�ܤ��롣
    const double p = g_Conf.depthRes / 512.0;
    
    // 8��ǥե���Ȥ�Ⱦ�¤Ȥ���ȡ�
    // �����Ⱦ�¤�q�ܤˤʤ�ȡ�����ι⤵�Ϥ��ΤޤޤʤΤǡ�
    // q�ܲ�Ĺ������ˤʤäƤ��ޤ���
    // �����ǡ�����ι⤵��q�ܤ��롣
    // ���Τޤ޷׻�����ȡ�q�ܤΥ�������Ƿ׻��������ˤʤ롣
    // ��äơ����Ф��������1/q�ܤ��롣
    const double q = g_Conf.riplRadius / 8.0;
*/    
    const double PIXEL_WIDTH = 1.0;     // �ԥ��������
    const double REFRACTION_INDEX = 1.33;    // ������ж���Ψ

    // �ơ��֥��ѥ�������(��������̢�g_Conf.depthRes^2 * 2byte(16bit))
    // ����ϡ����̤ι⤵�ȡ����Υԥ�����κ����ޤ��Ͼ岼�ι��㺹�˱ƶ������
    // Left < Right
    Uint16 *refractionTable = calloc(g_Conf.depthRes * g_Conf.depthRes, sizeof(Uint16));
    
    // �׻�����
    int i = 0, d = g_Conf.depthRes / 10;
    for (int heightDiff = 0; heightDiff < g_Conf.depthRes; heightDiff++)
    {
        // ���ͳѤ����
        double t1 = atan((heightDiff * 100.0 / (g_Conf.depthRes / 2.0)) / (2.0 * PIXEL_WIDTH));
        
        // ���޳Ѥ����
        double t2 = asin(sin(t1) / REFRACTION_INDEX);
        
        // �ƿ��̤ι⤵�˱�����������̤����
        // ����ϡ����̤ι⤵�ȡ����Υԥ�����κ����ޤ��Ͼ岼�ι��㺹�˱ƶ������
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
    
    // �ơ��֥�ؤΥݥ��󥿤��֤�
    return refractionTable;
}



/* ����ȯ���ؿ� */
void RippleOut(int x, int y)
{
    // �ȥǡ����κ�����ˤ�������֤˰�ư
    int incValue = (g_Conf.widthRes + 2) * ((y + 1) - g_Conf.riplRadius) +
                                           ((x + 1) - g_Conf.riplRadius);
    PosData *nextData = g_pNextData;
    PosData *crntData = g_pCrntData;
    nextData += incValue;
    crntData += incValue;
    PosData *riplData = g_pRiplData;
    
    // �����ȯ��������
    incValue = (g_Conf.widthRes + 2) - (2 * g_Conf.riplRadius + 1);
    for (int iy = -g_Conf.riplRadius; iy <= g_Conf.riplRadius; iy++)
    {
        for (int ix = -g_Conf.riplRadius; ix <= g_Conf.riplRadius; ix++)
        {
            if (*riplData != 0)
            {
                // �����ʤ顢���̤�­��
                if (((*nextData >= 0) && (*riplData < 0)) || ((*nextData <= 0) && (*riplData > 0)))
                    *nextData += *riplData;
                // Ʊ��������ι⤵����㤤�ʤ顢�����ޤǰ����夲��
                else if (*nextData < *riplData)
                    *nextData = *riplData;
                // Ʊ��������ι⤵����礭����Ʊ���Ǥ���ʤ顢���⤷�ʤ�
                else
                    ;
                
                // �����ʤ顢���̤�­��
                if (((*crntData >= 0) && (*riplData < 0)) || ((*crntData <= 0) && (*riplData > 0)))
                    *crntData += *riplData;
                // Ʊ��������ι⤵����㤤�ʤ顢�����ޤǰ����夲��
                else if (*crntData < *riplData)
                    *crntData = *riplData;
                // Ʊ��������ι⤵����礭����Ʊ���Ǥ���ʤ顢���⤷�ʤ�
                else
                    ;
            }
            
            // �ݥ��󥿤�ʤ��
            riplData++;
            nextData++;
            crntData++;
        }
        // ���˽񤭤���Ԥ�Ƭ�˰�ư
        nextData += incValue;
        crntData += incValue;
    }
}


/* ��������ؿ� */
void PaintToSurface(SDL_Surface *target)
{
    // �����꡼�󥵡��ե������å�
    SDL_LockSurface(target);
    SDL_LockSurface(g_pBgImage);
    
    // �ǽ�Υԥ�����ذ�ư
    PosData *waterData = g_pNextData;
    waterData += (g_Conf.widthRes + 2) + 1;
    
    // �ǽ�ιԤؤΥݥ��󥿤ǽ����
    Uint32 *scrLine = (Uint32 *)target->pixels;
    Uint32 *bgLine = (Uint32 *)g_pBgImage->pixels;
    
    // �ԥå������
    Uint32 scrPitch = target->pitch / sizeof(Uint32);
    Uint32 bgPitch = g_pBgImage->pitch / sizeof(Uint32);
    
    // ����
    for (int y = 0; y < g_Conf.heightRes; y++)
    {
        for (int x = 0; x < g_Conf.widthRes; x++)
        {
            int heightDiff;
            int dx;
            int dy;
            // X�������Τ����׻�
            heightDiff = waterData[x + 1] - waterData[x - 1];
            if (heightDiff >= 0)
                dx =  g_pRfraTbl[ heightDiff * g_Conf.depthRes + ((g_Conf.depthRes - 1) / 2 + waterData[x])];
            else
                dx = -g_pRfraTbl[-heightDiff * g_Conf.depthRes + ((g_Conf.depthRes - 1) / 2 + waterData[x])];
            
            // Y�������Τ����׻�
            heightDiff = waterData[x + (g_Conf.widthRes + 2)] - waterData[x - (g_Conf.widthRes + 2)];
            if (heightDiff >= 0)
                dy =  g_pRfraTbl[ heightDiff * g_Conf.depthRes + ((g_Conf.depthRes - 1) / 2 + waterData[x])];
            else
                dy = -g_pRfraTbl[-heightDiff * g_Conf.depthRes + ((g_Conf.depthRes - 1) / 2 + waterData[x])];
            
            // X�������Τ�����ϰ���˼��ޤ�褦��Ĵ��
            if (x + dx < 0)
                dx = -x;
            else if (x + dx > g_Conf.widthRes - 1)
                dx = g_Conf.widthRes - 1 - x;
            // Y�������Τ�����ϰ���˼��ޤ�褦��Ĵ��
            if (y + dy < 0)
                dy = -y;
            else if (y + dy > g_Conf.heightRes - 1)
                dy = g_Conf.heightRes - 1 - y;
            
            // ���줿���֤ο�������
            scrLine[x] = bgLine[x + dx + bgPitch * dy];
//            scrLine[x] = (waterData[x] + (g_Conf.depthRes - 1) / 2) * 255 / g_Conf.depthRes;
        }
        // ���ιԤ˥ݥ��󥿤�ʤ��
        scrLine += scrPitch;
        bgLine += bgPitch;
        waterData += g_Conf.widthRes + 2;
    }

    // �����꡼�󥵡��ե����Υ�å�����
    SDL_UnlockSurface(g_pBgImage);
    SDL_UnlockSurface(target);
}


