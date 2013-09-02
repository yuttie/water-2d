/*
 *******************************************************************************
 *
 *  step_fpu.c - MeshWater-2D-Float
 *
 *  Copyright  2004-2006  Yuta Taniguchi
 *******************************************************************************
 */

/* Include Files */
#include <stdbool.h>
#include "main.h"
#include "step_fpu.h"

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




