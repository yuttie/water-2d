/*
 *******************************************************************************
 *
 *  calcfunc.c - MeshWater-2D-Float
 *
 *  Copyright  2004-2006  Yuta Taniguchi
 *******************************************************************************
 */

/* Include Files */
#include <stdbool.h>
#include "main.h"
#include "calcfunc.h"

/* FPU */
void stepFPU(float dt) {
    float m = 1;
    float k = 1;
    // ���֤�®�٤ΰ����򹹿�
    for (int y = 1; y <= gConfig.virtualHeight; y++) {
        for (int x = 1; x <= gConfig.virtualWidth; x++) {
            int offset = gConfig.meshWidth * y + x;
            gPosition[offset] += gVelocity[offset] * dt +
                                 gForce[offset] * dt * dt / (2 * m);
            gVelocity[offset] += gForce[offset] * dt / (2 * m);
        }
    }
    // �������Ϥ����
    for (int y = 1; y <= gConfig.virtualHeight; y++) {
        for (int x = 1; x <= gConfig.virtualWidth; x++) {
            int offset = gConfig.meshWidth * y + x;
            gForce[offset]  = -k * (gPosition[offset] - gPosition[offset - 1]);
            gForce[offset] += -k * (gPosition[offset] - gPosition[offset + 1]);
            gForce[offset] += -k * (gPosition[offset] - gPosition[offset - gConfig.meshWidth]);
            gForce[offset] += -k * (gPosition[offset] - gPosition[offset + gConfig.meshWidth]);
        }
    }
    // ®�٤ΰ����򹹿�
    for (int y = 1; y <= gConfig.virtualHeight; y++) {
        for (int x = 1; x <= gConfig.virtualWidth; x++) {
            int offset = gConfig.meshWidth * y + x;
            gVelocity[offset] += gForce[offset] * dt / (2 * m);
        }
    }
}

/* MMX
void CalculateMMX()
{
    // ˰�½����Ѿ��/��������
    int maxHeight = (g_Conf.depthRes - 1) / 2;
    PosData upperLimit[4] = { maxHeight,  maxHeight,  maxHeight,  maxHeight};
    PosData lowerLimit[4] = {-maxHeight, -maxHeight, -maxHeight, -maxHeight};
    Sint16 v = g_Conf.attRate * 32768;
    Sint16 mulnum[4] = {v, v, v, v};
    
    // ������֡ߤ˰�ư
    // ����������
    // �ɡ߿���
    // �ɿ����
    // �ɿ����
    int SurfaceWidth = g_Conf.widthRes;
    int SurfaceHeight = g_Conf.heightRes;
    PosData *nextData = g_pNextData;
    PosData *crntData = g_pCrntData;
    PosData *prevData = g_pPrevData;
    nextData += 1 * (SurfaceWidth + 2) + 1;
    crntData += 1 * (SurfaceWidth + 2) + 1;
    prevData += 1 * (SurfaceWidth + 2) + 1;

    for (int y = 0; y < SurfaceHeight; y++)
    {
        for (int x = 0; x < SurfaceWidth; x += 4)
        {
            asm volatile (
                // ����������������׻�������������
                // crntData�ξ岼������û�
                "movq -2(%%esi), %%mm0\n"              // crntData�κ����ɤ߹���
                "paddsw 2(%%esi), %%mm0\n"             // crntData�α���û�
                "paddsw (%%esi, %%ebx, 2), %%mm0\n"    // crntData�β���û�
                "neg %%ebx\n"                          // ���ե��åȤ�2�������Ȥ�
                "paddsw (%%esi, %%ebx, 2), %%mm0\n"    // crntData�ξ��û�
                
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
            : "S" (crntData),              // ���ο���
              "D" (nextData),              // ����������
              "a" (prevData),              // �Ť�����
              "b" (SurfaceWidth + 2),      // ���̥ǡ�������
              "c" (upperLimit),            // �����С������å�������
              "d" (lowerLimit)             // ������������å�������
            : "memory");
            // <<< 1 - ����դ�16bit�������Ф���n/32768�ܥ��르�ꥺ�� >>>
            // n�ܤ��ơ����2�Х��ȤȲ���2�Х��Ȥ����롣
            // 1.n=32768��ݤ���(1�ܤ���)
            //                   SHHHHHHH LLLLLLLL
            // 2.���2�Х��Ȥ򺸥��եȡ�����2�Х��Ȥ򱦥��ե�
            // S0HHHHHH HLLLLLLL L0000000 00000000
            // 3.���2�Х��ȤȲ���2�Х��Ȥ�OR����
            // SHHHHHHH LLLLLLL0 00000000 0000000L
            // 4.����
            //                   SHHHHHHH LLLLLLLL
            // FPS=76.08
            asm volatile (
                // �����������������Ȥ��Ť�������������
                // mm0 
                "movq  (%%edi), %%mm0\n"     // ���̤ι⤵
                "movq  (%%esi), %%mm1\n"     // �ݤ���� n
                "movq   %%mm1,  %%mm2\n"     // �ݤ�����򥳥ԡ�
                "pmulhw %%mm0,  %%mm1\n"     // �ݤ��ƾ��2�Х��Ȥ����
                "pmullw %%mm0,  %%mm2\n"     // �ݤ��Ʋ���2�Х��Ȥ����
                "psllw  $1,     %%mm1\n"     // ���2�Х��Ȥ򺸤�1���ե�
                "psrlw  $15,    %%mm2\n"     // ����2�Х��Ȥ򱦤�15���ե�
                "por    %%mm2,  %%mm1\n"     // ��̥Х��ȤȲ��̥Х��Ȥ�OR����
                "movq   %%mm1, (%%edi)\n"    // ����˽��᤹
            :
            : "S" (mulnum),
              "D" (nextData)
            : "memory");
            // �ݥ��󥿤�ʤ��
            nextData += 4;
            crntData += 4;
            prevData += 4;
        }
        // �ݥ��󥿤�ʤ��
        nextData += 2;
        crntData += 2;
        prevData += 2;
    }
    
    // MMX��λ
    asm ("emms\n");
}
*/



