/*
 *******************************************************************************
 *
 *  step_sse.c - MeshWater-2D-Float
 *
 *  Copyright  2004-2006  Yuta Taniguchi
 *******************************************************************************
 */

/* Include Files */
#include <stdbool.h>
#include "main.h"
#include "step_sse.h"

/* SSE */
void stepSSE(float dt) {
    float m = 1;
    float k = 1;
    float inv2m = 1 / (2 * m);
    // 位置と速度の一部を更新
    float *pos = gPosition + gConfig.meshWidth * vMargin;
    float *vel = gVelocity + gConfig.meshWidth * vMargin;
    float *force = gForce + gConfig.meshWidth * vMargin;
    for (int y = vMargin; y < vMargin + gConfig.virtualHeight; y++) {
        for (int x = hMargin; x < hMargin + gConfig.virtualWidth; x += 4) {
            __asm__ __volatile__ (
                // メモリからレジスタにロード
                "movaps (%%eax,%%esi,4), %%xmm0\n"
                "movaps (%%ebx,%%esi,4), %%xmm1\n"
                "movaps (%%ecx,%%esi,4), %%xmm2\n"
                "movss  (%%edx), %%xmm3\n"
                "movss  (%%edi), %%xmm4\n"
                // dt, inv2mをベクトル化
                "shufps $0, %%xmm3, %%xmm3\n"
                "shufps $0, %%xmm4, %%xmm4\n"
                // 位置を計算
                "mulps  %%xmm3, %%xmm1\n"
                "mulps  %%xmm3, %%xmm2\n"
                "mulps  %%xmm3, %%xmm2\n"
                "mulps  %%xmm4, %%xmm2\n"
                "addps  %%xmm1, %%xmm0\n"
                "addps  %%xmm2, %%xmm0\n"
                "movaps %%xmm0, (%%eax,%%esi,4)\n"
                // メモリからレジスタにロード
                "movaps (%%ebx,%%esi,4), %%xmm1\n"
                "movaps (%%ecx,%%esi,4), %%xmm2\n"
                // 速度を部分的に計算
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
    // 新しい力を求める
    pos = gPosition + gConfig.meshWidth * vMargin;
    vel = gVelocity + gConfig.meshWidth * vMargin;
    force = gForce + gConfig.meshWidth * vMargin;
    for (int y = vMargin; y < vMargin + gConfig.virtualHeight; y++) {
        for (int x = hMargin; x < hMargin + gConfig.virtualWidth; x += 4) {
            __asm__ __volatile__ (
                // メモリからレジスタにロード
                "movaps (%%eax,%%edi,4), %%xmm0\n"
                "movups -4(%%eax,%%edi,4), %%xmm1\n"//左
                "movups 4(%%eax,%%edi,4), %%xmm2\n"//右
                "movl   %%edi, %%esi\n"
                "addl   %%edx, %%esi\n"
                "movaps (%%eax,%%esi,4), %%xmm3\n"//下
                "movl   %%edi, %%esi\n"
                "subl   %%edx, %%esi\n"
                "movaps (%%eax,%%esi,4), %%xmm4\n"//上
                "movss  (%%ecx), %%xmm5\n"
                // kをベクトル化
                "shufps $0, %%xmm5, %%xmm5\n"
                // 力を計算
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
    // 速度の一部を更新
    pos = gPosition + gConfig.meshWidth * vMargin;
    vel = gVelocity + gConfig.meshWidth * vMargin;
    force = gForce + gConfig.meshWidth * vMargin;
    for (int y = vMargin; y < vMargin + gConfig.virtualHeight; y++) {
        for (int x = hMargin; x < hMargin + gConfig.virtualWidth; x += 4) {
            __asm__ __volatile__ (
                // メモリからレジスタにロード
                "movaps (%%eax,%%edi,4), %%xmm0\n"
                "movaps (%%ebx,%%edi,4), %%xmm1\n"
                "movss  (%%ecx), %%xmm2\n"
                "movss  (%%edx), %%xmm3\n"
                // dt, inv2mをベクトル化
                "shufps $0, %%xmm2, %%xmm2\n"
                "shufps $0, %%xmm3, %%xmm3\n"
                // 速度を部分的に計算
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



