
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
 * Fix:  ������Ť������γ任��ʬ���ڤ�夲�ˤʤäƤ��ơ�
 * 
 * <<< Version 0.2.0  2004/03/27 12:10:05 >>>
 * Src:  �ץ�����������Ĥι�¤�ΤˤޤȤ᤿��
 * Src:  main�ؿ��򤹤ä��ꤵ������
 * New:  �ե륹���꡼��⡼�ɤǤμ¹Ԥ���ꤹ�뵯ư�����ץ������ɲä�����
 * Fix:  ���ޥơ��֥�λ��Ѥ�������̤�1/8�ǺѤ�褦�˽�����
 * New:  �طʲ����������ȹ⤵����Ƥ����Τ�ߤᡢ��ͳ�˻���Ǥ��뵯ư�����ץ������ɲä�����
 * New:  ��ưŪ�˻��ꤵ�줿�طʲ����Υ������򡢥�����ɥ��Υ������˥ꥵ��������褦�ˤ�����
 * New:  �ɥ�å����˹Ԥ���֤�ʬ�������ꤹ�뵯ư�����ץ������ɲä�����
 * Fix:  ���̤ι⤵�β����٤��Ѥ���ȡ����ޤ����������ʤ�Τ�����
 * Fix:  ������礭�����Ѥ���ȡ����ޤ����������ʤ�Τ�����
 * New:  ���̤ι⤵�β����٤�ľ�ܻ���Ǥ��뵯ư�����ץ������ɲä�����
 * Mod:  ���ץ����ο����ɲä˽��äƥإ�פ򹹿���
 * New:  ����å���������˿ʹԾ����򥿥��ȥ�С���ɽ������褦�ˤ�����
 * Fix:  ���ޥơ��֥�η׻��������٤��ä��Τ�����
 *
 * <<< Version 0.1.3  2004/02/28 15:55:20 >>>
 * Src:  �����ե����γ���̾��ؿ������������â��������������
 *
 * <<< Version 0.1.2  2004/02/27 14:28:17 >>>
 * New:  �إ�פ�ɽ�����뵯ư�����ץ�����ä�����
 *
 * <<< Version 0.1.1  2004/02/27 08:25:23 >>>
 * New: �ɥ�å����������ȯ��������դ��ˤ���
 * New: q�����ǽ�λ�Ǥ���褦�ˤ���
 *
 * <<< Version 0.1.0  2004/02/27 08:04:06 >>>
 * Fix:  ����βû��ˤ����̤��ǹ�����˰�¤�ʿó�����븽�ݤ�ľ������
 *
 * <<< Version 0.0.0  ????/??/?? ??:??:?? >>>
 * Info: Windows + Delphi�ˤ�ꡢJava�ˤ������ʤɤ򻲹ͤˤ��ƺ���������
 *       GDI��Ȥ���MMX��Ȥ鷺��������ǽ�������ä���
 *       ���θ塢Delphi�Υ���饤�󥢥���֥�����Ѥ��ơ����׷׻���ʬ��MMX��������
 *       Delphi����C����(Mingw)���ѹ�����Τ򵡤ˡ�SDL�����Ѥ��뤳�Ȥˤ�����
 *       ����η���ͽ��׻�������å��岽���Ƥ������Ȥǡ���®����Ԥä���
 *       �ֶ��ޤˤ�뤺��פη׻��򤭤���ȹԤ�������å��岽���Ƥ������Ȥˤ�����
 *       �ե�ͥ�ȿ�ͤ���������������������������򤤸���ȿ�ͤ�׻���������ʤΤǡ�
 *           �������Ǥ�̵�����ष��̵�������ɤ���Ƚ�Ǥ����Τǡ����������
*/


/* To Do
 9 �����˿��̤�����夫�ʤ�(0�ˤʤ�ʤ�)
 9 DrawWater�ؿ���ǳ��礷�����褹��Ƥμ¸�
 9 vorbose�⡼��
 5 �ѿ�̾
 4 �ɥ��ǥ���(�ߡ�ľ�����ԥ�����ñ�̤��Խ�)
 3 SSE���Υ��ݡ���
 2 ���̤˾����ɽ��(FPS��)
 1 ¾�Υǥ����ץ쥤�⡼�ɥ��ݡ���
 */


/* Max FPS(400x300) - debug
   55.34
   55.39 - inline
*/


/* Idea
 * FPS�κ����ͤ����Ǥ���褦�ˤ���
 * -v���ץ�������ǵ�ư����MaxWaterHeight�ʤɤΥ��ߥ�졼�����ǻȤ�����ͤ򥳥󥽡����ɽ������
 * �طʤβ������б����������䤹
 * 
*/


/* ����
 * ARGB-32bit�Τߥ��ݡ���
*/


/* ���󥯥롼�ɥե����� */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "SDL/SDL.h"
#include "imgscale.h"


/* �� */
typedef Sint16 PosData;
struct ProgramOption
{
    char *bgImgFile;         // �طʥ��᡼���Υե�����̾
    int resolution;          // ��������ʬ��ǽ
    int rippleRadius;        // ȯ�������������Ⱦ��(���᡼�������ꤵ��Ƥ��ʤ����˻���)
    int rippleHeight;        // ȯ������������ι⤵
    
    int waterWidth;          // ���̤���
    int waterHeight;         // ���̤ι⤵
    int windowWidth;         // ������ɥ�����
    int windowHeight;        // ������ɥ��ι⤵
    bool isFullScreen;       // �ե륹���꡼���ݤ�
    
    int ipDivide;            // ��������(InterPolating)��ʬ��ǹԤ���
};


/* �ؿ��Υץ�ȥ�������� */
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


/* ��� */
const double PI = 3.1415926535;    // �߼�Ψ


/* �ƥ����ȥǡ��� */
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


/* �����Х��ѿ� */
struct ProgramOption progOption;
PosData *newWater;          // ���̥ǡ���
PosData *curWater;          // ���̥ǡ���
PosData *oldWater;          // ���̥ǡ���
PosData *rippleData;        // ����ǡ���
Uint16 *refractionTable;    // ���ޤˤ���Ѱ��̥ơ��֥�
SDL_Surface *screen;
SDL_Surface *bg;


/* �ᥤ��ؿ� */
int main(int argc, char **argv)
{
    // �ץ��������
    InitProc(argc, argv);
    
    // FPS��¬���ѿ�
    Uint32 startTime = 0;
    Uint32 endTime = 0;
    Uint32 frameCount = 0;
    
    // ̵�¥롼��
    while(!EventProc())
    {
        // ���̤����褹��
        DrawWater();

        // ɽ���򹹿�
        SDL_UpdateRect(screen, 0, 0, 0, 0);
        
        // ���̤������ؤ���
        PosData *tmp = oldWater;
        oldWater = curWater;
        curWater = newWater;
        newWater = tmp;
        
        // ���̤�׻�
        CalcWater();
        
        // FPS��¬
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
    
    // �ץ���ཪλ����
    ExitProc();
    
    return 0;
}


/* �ץ���������ؿ� */
void InitProc(int argc, char **argv)
{
    // �ץ���४�ץ����Υǥե�����ͤ�����
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
    
    // ��������
    if (ParseArgument(argc, argv))
    {
        // �Ƽ�����ɸ����Ϥ�ɽ������
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
    
    // SDL��������ƥ����꡼�󥵡��ե��������
    InitSDL();
    
    // �طʲ����ɤ߹���
    SDL_Surface *tmpBg = SDL_LoadBMP(progOption.bgImgFile);
    if (tmpBg == NULL)
    {
        fprintf(stderr, "�طʲ����ե����뤬�ɤ߹���ޤ���\n");
        exit(EXIT_FAILURE);
    }
    
    // �طʥ����ե����򥹥��꡼�󥵡��ե����η������Ѵ�
    tmpBg = SDL_ConvertSurface(tmpBg, screen->format, SDL_SWSURFACE);
    
    // �ɤ߹�����طʤ���̤��礭���˹�碌��
    bg = SDL_CreateRGBSurface(SDL_SWSURFACE, progOption.waterWidth, progOption.waterHeight, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    ScaleCopySurface(SM_BI_LINEAR, tmpBg, bg);
    
    // ���̥ǡ����������
    newWater = calloc((progOption.waterWidth + 2) * (progOption.waterHeight + 2), sizeof(PosData));
    curWater = calloc((progOption.waterWidth + 2) * (progOption.waterHeight + 2), sizeof(PosData));
    oldWater = calloc((progOption.waterWidth + 2) * (progOption.waterHeight + 2), sizeof(PosData));
    
    // ����ǡ�������
    rippleData = CreateRippleData();
    
    // ���ޥơ��֥�����
    refractionTable = CreateRefractionTable();
    
    // ���ޥơ��֥륢����������ɬ�פʥ��ե��åȤ�û����Ƥ���
//    refractionTable += (progOption.resolution * (MaxWaterHeight - MinWaterHeight + 1)) - MinWaterHeight;
}


/* ���٥�Ƚ����ؿ� */
inline bool EventProc()
{
    int radius = progOption.rippleRadius;
    
    bool exitFlag = false;    // ��λ����륤�٥�Ȥ�ͭ�뤫�ɤ����Υե饰
    static int preCursorX;    // ���Υ��٥��ȯ�����Υޥ������������X��ɸ
    static int preCursorY;    // ���Υ��٥��ȯ�����Υޥ������������Y��ɸ
    SDL_Event event;          // ���٥�Ȥ˴ؤ���������빽¤��(������?)
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
            {   // ���������󥤥٥��
                switch (event.key.keysym.sym)
                {
                    case SDLK_q:
                    {   // q����
                        exitFlag = true;
                        break;
                    }
                    default:
                    {   // ����¾�Υ���(�ϥ�ɥ뤵��ʤ�����)
                        break;
                    }
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {   // �ޥ����ܥ�������󥤥٥��
                if (event.button.state == SDL_PRESSED)
                {
                    if ((event.button.x > radius) && (event.button.x < progOption.waterWidth - radius) &&
                        (event.button.y > radius) && (event.button.y < progOption.waterHeight - radius))
                    {
                        // �����ȯ��
                        RippleOut(event.button.x, event.button.y);
                        preCursorX = event.motion.x;
                        preCursorY = event.motion.y;
                    }
                }
                break;
            }
            case SDL_MOUSEMOTION:
            {   // �ޥ������������ư���٥��
                if (event.motion.state == SDL_PRESSED)
                {
                    if ((event.motion.x > radius) && (event.motion.x < progOption.waterWidth - radius) &&
                        (event.motion.y > radius) && (event.motion.y < progOption.waterHeight - radius))
                    {
                        // �����ȯ��(��֤���)
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
            {   // ��λ���٥��
                exitFlag = true;
                break;
            }
        }
    }
    return exitFlag;
}


/* �ץ���ཪλ���θ���� */
void ExitProc()
{
    // ���ޥơ��֥륢����������ɬ�פʥ��ե��åȤ򸺻����Ƥ���
//    refractionTable -= (-(MinWaterHeight - MaxWaterHeight) * (MaxWaterHeight - MinWaterHeight + 1)) - MinWaterHeight;
    
    // ����β���
    SDL_FreeSurface(bg);
    free(newWater);
    free(curWater);
    free(oldWater);
    free(rippleData);
    free(refractionTable);
    
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
                {   // �Ȥ�����ɽ�����ƽ�λ
                    printf(HELP_MESSAGE); exit(EXIT_SUCCESS);
                    break;
                }
                case 'v':
                {   // ��Ĺ��ɽ����Ԥ�
                    isVerboseMode = true;
                    break;
                }
                case 'f':
                {   // �ե륹���꡼��⡼�ɤ�����
                    progOption.isFullScreen = true;
                    break;
                }
                case 'r':
                {   // ������礭�������
                    if (argv[i][2] == '\0') progOption.rippleRadius = atoi(&argv[++i][0]);
                    else                    progOption.rippleRadius = atoi(&argv[i][2]);
                    break;
                }
                case 'd':
                {   // ��ֻ���ʬ��������
                    if (argv[i][2] == '\0') progOption.ipDivide = atoi(&argv[++i][0]);
                    else                    progOption.ipDivide = atoi(&argv[i][2]);
                    break;
                }
                case 'b':
                {   // �طʤΥե�����̾�����
                    if (argv[i][2] == '\0') progOption.bgImgFile = &argv[++i][0];
                    else                    progOption.bgImgFile = &argv[i][2];
                    break;
                }
                case 's':
                {   // �׻���ο��̤Υ����������
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
                {   // ������Ѳ�ǽ��(KBñ��)�����
                    // ���̤ι⤵������(������)�˱ƶ�����
                    int mem;
                    if (argv[i][2] == '\0') mem = atoi(&argv[++i][0]);
                    else                    mem = atoi(&argv[i][2]);
                    
                    // ���̤ι⤵�˴�����������
                    // �Ȥη׻����ˡ��岼������­������
                    // (PosData(int16_t)�κ�����32768)��4=8192���
                    // �ƥԥ�����ι⤵�κ����ͤ�8191���Ǿ��ͤ�-8191
                    // ���ޥơ��֥��ѥ���������(��������̢�progOption.resolution^2 * 2byte(16bit))
                    progOption.resolution = (int)(sqrt(mem * 1024 / 2.0));
                    if (progOption.resolution > 8191 * 2 + 1)
                        progOption.resolution = 8192 * 2 +1;
                    
                    //
//                    printf("%d\n", progOption.resolution);
                    
                    // ����ι⤵��Ʒ׻�
                    progOption.rippleHeight = (progOption.resolution - 1) / 2;
                    
                    break;
                }
                case 'p':
                {   // ���̤ι⤵������(������)�����
                    // ��������̤˱ƶ�����
                    if (argv[i][2] == '\0') progOption.resolution = atoi(&argv[++i][0]);
                    else                    progOption.resolution = atoi(&argv[i][2]);
                    
                    // ����ι⤵��Ʒ׻�
                    progOption.rippleHeight = (progOption.resolution - 1) / 2;
                    
                    break;
                }
                default:
                {   // ����¾�ξ��
                    fprintf(stderr, "�����ʰ������ޤޤ�Ƥ��ޤ���\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    
    // ��Ĺ��ɽ����Ԥ����ݤ����֤�
    return isVerboseMode;
}


/* CPUID̿���̵ͭ��ǧ�ؿ� */
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

/* SDL������ؿ� */
void InitSDL()
{
    // SDL������
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        // ���顼��å�������ɽ��
        fprintf(stderr, "������˼��Ԥ��ޤ�����%s\n", SDL_GetError());
        
        // �ץ�����λ
        exit(EXIT_FAILURE);
    }
    
    // �ǥ����ץ쥤�ǥХ����ν�����ե饰������
    Uint32 screenFlags = SDL_SWSURFACE;
    if (progOption.isFullScreen) screenFlags |= SDL_FULLSCREEN;
    
    // �ǥ����ץ쥤�ǥХ���������
    screen = SDL_SetVideoMode(progOption.windowWidth, progOption.windowHeight, 32, screenFlags);
    
    // ������˼��Ԥ����顢���顼��å�������ɽ�����ƽ�λ
    if (screen == NULL)
    {
        // ���顼��å�������ɽ��
        fprintf(stderr, "�ӥǥ��ν�����˼��Ԥ��ޤ�����%s\n", SDL_GetError());
        
        // SDL��λ
        SDL_Quit();
        
        // �ץ�����λ
        exit(EXIT_FAILURE);
    }
}


/* ����ǡ��������ؿ� */
PosData *CreateRippleData()
{
    int radius = progOption.rippleRadius;
    // ����ǡ�������
    PosData *rippleData = calloc((2 * radius + 1) * (2 * radius + 1), sizeof(PosData));
    PosData *pData = rippleData;
    
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
                *pData = (PosData)((cos(t) + 1) * progOption.rippleHeight / 2);
            }
            else
            {   // �߳�
                *pData = 0;
            }
            pData++;
        }
    }
    return rippleData;
}


/* ���ޥơ��֥�����ؿ� */
Uint16 *CreateRefractionTable()
{
    // ��������ʬ��ǽ��p�ܤˤʤ�ȡ�����ι⤵��p�ܤˤʤ�
    // �����ǥԥ����������p�ܤˤ��ʤ��ȡ�p�ܽ�Ĺ������ˤʤäƤ��ޤ���
    // ����ǤϺ���Τǡ��ԥ����������p�ܤ��롣
    // �����ޤǤǡ�����Ū��p�ܤΥ�������Ƿ׻��������ˤʤ롣
    // �����ǡ����Ф��������1/p�ܤ��롣
    const double p = progOption.resolution / 512.0;
    
    // �����Ⱦ�¤�q�ܤˤʤ�ȡ�����ι⤵�Ϥ��ΤޤޤʤΤǡ�
    // q�ܲ�Ĺ������ˤʤäƤ��ޤ���
    // �����ǡ�����ι⤵��q�ܤ��롣
    // ���Τޤ޷׻�����ȡ�q�ܤΥ�������Ƿ׻��������ˤʤ롣
    // ��äơ����Ф��������1/q�ܤ��롣
    const double q = progOption.rippleRadius / 8.0;
    
    const double PIXEL_WIDTH = 64.0 * p;         // �ԥ��������
    const double REFRACTION_INDEX = 1.33;    // ������ж���Ψ

    // �ơ��֥��ѥ�������(��������̢�progOption.resolution^2 * 2byte(16bit))
    // ����ϡ����̤ι⤵�ȡ����Υԥ�����κ����ޤ��Ͼ岼�ι��㺹�˱ƶ������
    // Left < Right
    Uint16 *refractionTable = calloc(progOption.resolution * progOption.resolution, sizeof(Uint16));
    
    // �׻�����
    int i = 0, d = progOption.resolution / 10;
    for (int heightDiff = 0; heightDiff < progOption.resolution; heightDiff++)
    {
        // ���ͳѤ����
        double t1 = atan(q * heightDiff / (2 * PIXEL_WIDTH));
        
        // ���޳Ѥ����
        double t2 = asin(sin(t1) / REFRACTION_INDEX);
        
        // �ƿ��̤ι⤵�˱�����������̤����
        // ����ϡ����̤ι⤵�ȡ����Υԥ�����κ����ޤ��Ͼ岼�ι��㺹�˱ƶ������
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
    
    // �ơ��֥�ؤΥݥ��󥿤��֤�
    return refractionTable;
}


/* ���̷׻��ؿ� */
inline void CalcWater()
{
    // ˰�½����Ѿ��/��������
    int maxHeight = (progOption.resolution - 1) / 2;
    PosData upperLimit[4] = { maxHeight,  maxHeight,  maxHeight,  maxHeight};
    PosData lowerLimit[4] = {-maxHeight, -maxHeight, -maxHeight, -maxHeight};
    Sint16 addnum[4] = {63, 63, 63, 63};
    Sint16 mulnum[4] = {32256, 32256, 32256, 32256};
    
    // ������֡ߤ˰�ư
    // ����������
    // �ɡ߿���
    // �ɿ����
    // �ɿ����
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
                // ����������������׻�������������
                // curWater�ξ岼������û�
                "movq -2(%%esi), %%mm0\n"                            // curWater�κ����ɤ߹���
                "paddsw 2(%%esi), %%mm0\n"                           // curWater�α���û�
                "paddsw (%%esi, %%ebx, 2), %%mm0\n"                  // curWater�β���û�
                "neg %%ebx\n" "paddsw (%%esi, %%ebx, 2), %%mm0\n"    // curWater�ξ��û�
                
                // �û�������Τ�2ʬ��1�ˤ����0�����˴ݤ���ǡ�
                "movq %%mm0, %%mm1\n"      // ���ԡ�
                "psrlw $15, %%mm1\n"       // �Ǿ�̥ӥåȤ����
                "paddsw %%mm1, %%mm0\n"    // �Ǿ�̥ӥåȤ�û�
                "psraw $1, %%mm0\n"        // ���ѱ����եȱ黻
                
                // oldWater������򸺻�
                "psubsw (%%eax), %%mm0\n"
                
                // ����������������˰�½�����������
                // ��±ۤ������å���������ɤ߹���
                "movq (%%ecx), %%mm5\n"
                "movq %%mm0, %%mm6\n"       // ��ӤǾ�񤭤����Τǥ��ԡ�
                
                // ��¤�ۤ��Ƥ���ս��õ���ʥޥ���������
                "pcmpgtw %%mm5, %%mm0\n"    // ��¤�ۤ��Ƥ���꤬1��¾��0�ˤʤ�
                
                // ˰�¤����� 
                "pand %%mm0, %%mm5\n"       // ������±ۤ��ս�Τߤˤ���
                "pandn %%mm6, %%mm0\n"      // ��±ۤ��ս��0�ˤ���
                "por %%mm5, %%mm0\n"        // �����¹���
                
                // ���±ۤ������å���������ɤ߹���
                "movq (%%edx), %%mm5\n"
                "movq %%mm5, %%mm6\n"       // ��ӤǾ�񤭤����Τǥ��ԡ�
                
                // ���¤�ۤ��Ƥ���ս��õ���ʥޥ���������
                "pcmpgtw %%mm0, %%mm6\n"    // ���¤�ۤ��Ƥ���꤬1�ˤʤ�
                
                // ˰�¤�����
                "pand %%mm6, %%mm5\n"       // ����򲼸±ۤ��ս�Τߤˤ���
                "pandn %%mm0, %%mm6\n"      // ���±ۤ��ս��0�ˤ���
                "por %%mm5, %%mm6\n"        // �����¹���
                
                // ����˽��᤹
                "movq %%mm6, (%%edi)\n"
            :
            : "S" (curWater),              // ���ο���
              "D" (newWater),              // ����������
              "a" (oldWater),              // �Ť�����
              "b" (SurfaceWidth + 2),      // ���̥ǡ�������
              "c" (upperLimit),            // �����С������å�������
              "d" (lowerLimit)             // ������������å�������
            : "memory");
#ifdef MUL
            // 76.08
            asm volatile (
                // �����������������Ȥ��Ť�������������
                // p/256�ܤˤ��롣p��ݤ��ơ����8�ӥåȤΤ߼�����
                // ����8�ӥåȤ򱦥��եȤ����Τ�Ʊ��
                "movq (%%edi), %%mm0\n"
                "movq (%%esi), %%mm1\n"
                "movq %%mm1, %%mm2\n"
                "pmulhw %%mm0, %%mm1\n"
                "pmullw %%mm0, %%mm2\n"
                "psllw $1, %%mm1\n"
                "psrlw $15, %%mm2\n"
                "por %%mm2, %%mm1\n"
                "movq %%mm1, (%%edi)\n"    // ����˽��᤹
            :
            : "S" (mulnum),
              "D" (newWater)
            : "memory");
#else
            // 75.66
            asm volatile (
                // �����������������Ȥ��Ť�������������
                // Xʬ��X-1�ܤˤ���(Xʬ��1�����)
                "movq (%%edi), %%mm0\n"    // ���ԡ�(Xʬ��X-1�ˤʤ�)
                "movq %%mm0, %%mm1\n"      // ���ԡ�(Xʬ��1�ˤʤ�)
                "movq %%mm0, %%mm2\n"      // �Ǿ�̥ӥå������
                "movq (%%esi), %%mm3\n"    // 63
                "psrlw $15, %%mm2\n"       // �Ǿ�̥ӥåȤ����
                "paddsw %%mm2, %%mm1\n"    // �Ǿ�̥ӥåȤ�û�
                "paddsw %%mm3, %%mm1\n"    // X�ǳ�����ˡ�(X-1)��­���Ƥ���(�������ʲ����ڤ�夲�뤿��)
                "psraw $6, %%mm1\n"        // ���ѱ����եȱ黻(Xʬ��1�ܤˤ���)
                "psubsw %%mm1, %%mm0\n"    // Xʬ��1�ܤ������Xʬ��X-1�ܤˤ���
                "movq %%mm0, (%%edi)\n"    // ����˽��᤹
            :
            : "S" (addnum),
              "D" (newWater)
            : "memory");
#endif
            // �ݥ��󥿤�ʤ��
            newWater += 4;
            curWater += 4;
            oldWater += 4;
        }
        // �ݥ��󥿤�ʤ��
        newWater += 2;
        curWater += 2;
        oldWater += 2;
    }

    // MMX��λ
    asm ("emms\n");
}


/* ����ȯ���ؿ� */
inline void RippleOut(int x, int y)
{
    int SurfaceWidth = progOption.waterWidth;
//    int SurfaceHeight = progOption.waterHeight;
    int radius = progOption.rippleRadius;
    
    // �ȥǡ����κ�����ˤ�������֤˰�ư
    int incValue = (SurfaceWidth + 2) * ((y + 1) - radius) + ((x + 1) - radius);
    PosData *tmp = newWater;
    PosData *newWater = tmp;
    tmp = curWater;
    PosData *curWater = tmp;
    newWater += incValue;
    curWater += incValue;
    tmp = rippleData;
    PosData *rippleData = tmp;
    
    // �����ȯ��������
    incValue = (SurfaceWidth + 2) - (2 * radius + 1);
    for (int iy = -radius; iy <= radius; iy++)
    {
        for (int ix = -radius; ix <= radius; ix++)
        {
            if (*rippleData != 0)
            {
                // �����ʤ顢���̤�­��
                if (((*newWater >= 0) && (*rippleData < 0)) || ((*newWater <= 0) && (*rippleData > 0)))
                    *newWater += *rippleData;
                // Ʊ��������ι⤵����㤤�ʤ顢�����ޤǰ����夲��
                else if (*newWater < *rippleData)
                    *newWater = *rippleData;
                // Ʊ��������ι⤵����礭����Ʊ���Ǥ���ʤ顢���⤷�ʤ�
                else
                    ;
                
                // �����ʤ顢���̤�­��
                if (((*curWater >= 0) && (*rippleData < 0)) || ((*curWater <= 0) && (*rippleData > 0)))
                    *curWater += *rippleData;
                // Ʊ��������ι⤵����㤤�ʤ顢�����ޤǰ����夲��
                else if (*curWater < *rippleData)
                    *curWater = *rippleData;
                // Ʊ��������ι⤵����礭����Ʊ���Ǥ���ʤ顢���⤷�ʤ�
                else
                    ;
            }
            
            // �ݥ��󥿤�ʤ��
            rippleData++;
            newWater++;
            curWater++;
        }
        // ���˽񤭤���Ԥ�Ƭ�˰�ư
        newWater += incValue;
        curWater += incValue;
    }
}


/* ��������ؿ� */
inline void DrawWater()
{
    int SurfaceWidth = progOption.waterWidth;
    int SurfaceHeight = progOption.waterHeight;
    int Resolution = progOption.resolution;
    
    // �����꡼�󥵡��ե������å�
    SDL_LockSurface(screen);
    SDL_LockSurface(bg);
    
    // �ǽ�Υԥ�����ذ�ư
    PosData *waterData = newWater;
    waterData += (SurfaceWidth + 2) + 1;
    
    // �ǽ�ιԤؤΥݥ��󥿤ǽ����
    Uint32 *scrLine = (Uint32 *)screen->pixels;
    Uint32 *bgLine = (Uint32 *)bg->pixels;
    
    // �ԥå������
    Uint32 scrPitch = screen->pitch / sizeof(Uint32);
    Uint32 bgPitch = bg->pitch / sizeof(Uint32);
    
    // ����
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
            
            // �ϰ���˼��ޤ�褦��Ĵ��
            if (x + dx < 0)                         dx = -x;
            else if (x + dx > SurfaceWidth - 1)     dx = SurfaceWidth - 1 - x;
            if (y + dy < 0)                         dy = -y;
            else if (y + dy > SurfaceHeight - 1)    dy = SurfaceHeight - 1 - y;
            
            // ���줿���֤ο�������
            scrLine[x] = bgLine[x + bgPitch * dy + dx];
//            scrLine[x] = (waterData[x] + (Resolution - 1) / 2) * 255 / Resolution;
        }
        // ���ιԤ˥ݥ��󥿤�ʤ��
        scrLine += scrPitch;
        bgLine += bgPitch;
        waterData += SurfaceWidth + 2;
    }

    // �����꡼�󥵡��ե����Υ�å�����
    SDL_UnlockSurface(bg);
    SDL_UnlockSurface(screen);
}


