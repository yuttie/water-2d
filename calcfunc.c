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
    for (int y = vMargin; y < vMargin + gConfig.virtualHeight; y++) {
        for (int x = hMargin; x < hMargin + gConfig.virtualWidth; x++) {
            int offset = gConfig.meshWidth * y + x;
            gPosition[offset] += gVelocity[offset] * dt +
                                 gForce[offset] * dt * dt / (2 * m);
            gVelocity[offset] += gForce[offset] * dt / (2 * m);
        }
    }
    // �������Ϥ����
    for (int y = vMargin; y < vMargin + gConfig.virtualHeight; y++) {
        for (int x = hMargin; x < hMargin + gConfig.virtualWidth; x++) {
            int offset = gConfig.meshWidth * y + x;
            gForce[offset]  = -k * (gPosition[offset] - gPosition[offset - 1]);
            gForce[offset] += -k * (gPosition[offset] - gPosition[offset + 1]);
            gForce[offset] += -k * (gPosition[offset] - gPosition[offset - gConfig.meshWidth]);
            gForce[offset] += -k * (gPosition[offset] - gPosition[offset + gConfig.meshWidth]);
        }
    }
    // ®�٤ΰ����򹹿�
    for (int y = vMargin; y < vMargin + gConfig.virtualHeight; y++) {
        for (int x = hMargin; x < hMargin + gConfig.virtualWidth; x++) {
            int offset = gConfig.meshWidth * y + x;
            gVelocity[offset] += gForce[offset] * dt / (2 * m);
        }
    }
}

/* SSE */
void stepSSE(float dt) {
    float m = 1;
    float k = 1;
    float inv2m = 1 / (2 * m);
    // ���֤�®�٤ΰ����򹹿�
    float *pos = gPosition + gConfig.meshWidth * vMargin;
    float *vel = gVelocity + gConfig.meshWidth * vMargin;
    float *force = gForce + gConfig.meshWidth * vMargin;
    for (int y = vMargin; y < vMargin + gConfig.virtualHeight; y++) {
        for (int x = hMargin; x < hMargin + gConfig.virtualWidth; x += 4) {
            asm volatile (
                // ���꤫��쥸�����˥���
                "movaps (%%eax,%%esi,4), %%xmm0\n"
                "movaps (%%ebx,%%esi,4), %%xmm1\n"
                "movaps (%%ecx,%%esi,4), %%xmm2\n"
                "movss  (%%edx), %%xmm3\n"
                "movss  (%%edi), %%xmm4\n"
                // dt, inv2m��٥��ȥ벽
                "shufps $0, %%xmm3, %%xmm3\n"
                "shufps $0, %%xmm4, %%xmm4\n"
                // ���֤�׻�
                "mulps  %%xmm3, %%xmm1\n"
                "mulps  %%xmm3, %%xmm2\n"
                "mulps  %%xmm3, %%xmm2\n"
                "mulps  %%xmm4, %%xmm2\n"
                "addps  %%xmm1, %%xmm0\n"
                "addps  %%xmm2, %%xmm0\n"
                "movaps %%xmm0, (%%eax,%%esi,4)\n"
                // ���꤫��쥸�����˥���
                "movaps (%%ebx,%%esi,4), %%xmm1\n"
                "movaps (%%ecx,%%esi,4), %%xmm2\n"
                // ®�٤���ʬŪ�˷׻�
                "mulps  %%xmm3, %%xmm2\n"
                "mulps  %%xmm4, %%xmm2\n"
                "addps  %%xmm2, %%xmm1\n"
                "movaps %%xmm1, (%%ebx,%%esi,4)\n"
            :
            : "a" (pos), "b" (vel), "c" (force),
              "d" (&dt), "D" (&inv2m), "S" (x)
            : "memory"
            );
        }
        pos += gConfig.meshWidth;
        vel += gConfig.meshWidth;
        force += gConfig.meshWidth;
    }
    // �������Ϥ����
    pos = gPosition + gConfig.meshWidth * vMargin;
    vel = gVelocity + gConfig.meshWidth * vMargin;
    force = gForce + gConfig.meshWidth * vMargin;
    for (int y = vMargin; y < vMargin + gConfig.virtualHeight; y++) {
        for (int x = hMargin; x < hMargin + gConfig.virtualWidth; x += 4) {
            asm volatile (
                // ���꤫��쥸�����˥���
                "movaps (%%eax,%%edi,4), %%xmm0\n"
                "movups -4(%%eax,%%edi,4), %%xmm1\n"//��
                "movups 4(%%eax,%%edi,4), %%xmm2\n"//��
                "movl   %%edi, %%esi\n"
                "addl   %%edx, %%esi\n"
                "movaps (%%eax,%%esi,4), %%xmm3\n"//��
                "movl   %%edi, %%esi\n"
                "subl   %%edx, %%esi\n"
                "movaps (%%eax,%%esi,4), %%xmm4\n"//��
                "movss  (%%ecx), %%xmm5\n"
                // k��٥��ȥ벽
                "shufps $0, %%xmm5, %%xmm5\n"
                // �Ϥ�׻�
                "addps  %%xmm2, %%xmm1\n"
                "addps  %%xmm3, %%xmm1\n"
                "addps  %%xmm4, %%xmm1\n"
                "subps  %%xmm0, %%xmm1\n"
                "subps  %%xmm0, %%xmm1\n"
                "subps  %%xmm0, %%xmm1\n"
                "subps  %%xmm0, %%xmm1\n"
                "mulps  %%xmm5, %%xmm1\n"
                "movaps %%xmm1, (%%ebx,%%edi,4)\n"
            :
            : "a" (pos), "b" (force),
              "c" (&k), "d" (gConfig.meshWidth), "D" (x)
            : "memory"
            );
        }
        pos += gConfig.meshWidth;
        vel += gConfig.meshWidth;
        force += gConfig.meshWidth;
    }
    // ®�٤ΰ����򹹿�
    pos = gPosition + gConfig.meshWidth * vMargin;
    vel = gVelocity + gConfig.meshWidth * vMargin;
    force = gForce + gConfig.meshWidth * vMargin;
    for (int y = vMargin; y < vMargin + gConfig.virtualHeight; y++) {
        for (int x = hMargin; x < hMargin + gConfig.virtualWidth; x += 4) {
            asm volatile (
                // ���꤫��쥸�����˥���
                "movaps (%%eax,%%edi,4), %%xmm0\n"
                "movaps (%%ebx,%%edi,4), %%xmm1\n"
                "movss  (%%ecx), %%xmm2\n"
                "movss  (%%edx), %%xmm3\n"
                // dt, inv2m��٥��ȥ벽
                "shufps $0, %%xmm2, %%xmm2\n"
                "shufps $0, %%xmm3, %%xmm3\n"
                // ®�٤���ʬŪ�˷׻�
                "mulps  %%xmm2, %%xmm1\n"
                "mulps  %%xmm3, %%xmm1\n"
                "addps  %%xmm1, %%xmm0\n"
                "movaps %%xmm0, (%%eax,%%edi,4)\n"
            :
            : "a" (vel), "b" (force),
              "c" (&dt), "d" (&inv2m), "D" (x)
            : "memory"
            );
        }
        pos += gConfig.meshWidth;
        vel += gConfig.meshWidth;
        force += gConfig.meshWidth;
    }
}



