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
    // 位置と速度の一部を更新
    for (int y = 1; y <= gConfig.virtualHeight; y++) {
        for (int x = 1; x <= gConfig.virtualWidth; x++) {
            int offset = gConfig.meshWidth * y + x;
            gPosition[offset] += gVelocity[offset] * dt +
                                 gForce[offset] * dt * dt / (2 * m);
            gVelocity[offset] += gForce[offset] * dt / (2 * m);
        }
    }
    // 新しい力を求める
    for (int y = 1; y <= gConfig.virtualHeight; y++) {
        for (int x = 1; x <= gConfig.virtualWidth; x++) {
            int offset = gConfig.meshWidth * y + x;
            gForce[offset]  = -k * (gPosition[offset] - gPosition[offset - 1]);
            gForce[offset] += -k * (gPosition[offset] - gPosition[offset + 1]);
            gForce[offset] += -k * (gPosition[offset] - gPosition[offset - gConfig.meshWidth]);
            gForce[offset] += -k * (gPosition[offset] - gPosition[offset + gConfig.meshWidth]);
        }
    }
    // 速度の一部を更新
    for (int y = 1; y <= gConfig.virtualHeight; y++) {
        for (int x = 1; x <= gConfig.virtualWidth; x++) {
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
    // 位置と速度の一部を更新
    float *pos = gPosition + gConfig.meshWidth * 1;
    float *vel = gVelocity + gConfig.meshWidth * 1;
    float *force = gForce + gConfig.meshWidth * 1;
    for (int y = 1; y <= gConfig.virtualHeight; y++) {
        for (int x = 1; x <= gConfig.virtualWidth; x += 4) {
            asm volatile (
                // メモリからレジスタにロード
                "movups (%%eax,%%esi,4), %%xmm0\n"
                "movups (%%ebx,%%esi,4), %%xmm1\n"
                "movups (%%ecx,%%esi,4), %%xmm2\n"
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
                "movups %%xmm0, (%%eax,%%esi,4)\n"
                // メモリからレジスタにロード
                "movups (%%ebx,%%esi,4), %%xmm1\n"
                "movups (%%ecx,%%esi,4), %%xmm2\n"
                // 速度を部分的に計算
                "mulps  %%xmm3, %%xmm2\n"
                "mulps  %%xmm4, %%xmm2\n"
                "addps  %%xmm2, %%xmm1\n"
                "movups %%xmm1, (%%ebx,%%esi,4)\n"
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
    pos = gPosition + gConfig.meshWidth * 1;
    vel = gVelocity + gConfig.meshWidth * 1;
    force = gForce + gConfig.meshWidth * 1;
    for (int y = 1; y <= gConfig.virtualHeight; y++) {
        for (int x = 1; x <= gConfig.virtualWidth; x++) {
            asm volatile (
                // メモリからレジスタにロード
                "movups (%%eax,%%edi,4), %%xmm0\n"
                "movups -4(%%eax,%%edi,4), %%xmm1\n"//左
                "movups 4(%%eax,%%edi,4), %%xmm2\n"//右
                "movl   %%edi, %%esi\n"
                "addl   %%edx, %%esi\n"
                "movups (%%eax,%%esi,4), %%xmm3\n"//下
                "movl   %%edi, %%esi\n"
                "subl   %%edx, %%esi\n"
                "movups (%%eax,%%esi,4), %%xmm4\n"//上
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
                "movups %%xmm1, (%%ebx,%%edi,4)\n"
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
    pos = gPosition + gConfig.meshWidth * 1;
    vel = gVelocity + gConfig.meshWidth * 1;
    force = gForce + gConfig.meshWidth * 1;
    for (int y = 1; y <= gConfig.virtualHeight; y++) {
        for (int x = 1; x <= gConfig.virtualWidth; x++) {
            asm volatile (
                // メモリからレジスタにロード
                "movups (%%eax,%%edi,4), %%xmm0\n"
                "movups (%%ebx,%%edi,4), %%xmm1\n"
                "movss  (%%ecx), %%xmm2\n"
                "movss  (%%edx), %%xmm3\n"
                // dt, inv2mをベクトル化
                "shufps $0, %%xmm2, %%xmm2\n"
                "shufps $0, %%xmm3, %%xmm3\n"
                // 速度を部分的に計算
                "mulps  %%xmm2, %%xmm1\n"
                "mulps  %%xmm3, %%xmm1\n"
                "addps  %%xmm1, %%xmm0\n"
                "movups %%xmm0, (%%eax,%%edi,4)\n"
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



