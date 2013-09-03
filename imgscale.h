/*
 *******************************************************************************
 *
 *  imgscale.h - *
 *
 *  Copyright (C) 2004-2006 Yuta Taniguchi
 *******************************************************************************
 */

#ifndef __IMGSCALE_H
#define __IMGSCALE_H
/* 列挙型 */
typedef enum __ScalingMethod {
    SM_SIMPLE,
    SM_BI_LINEAR
} ScalingMethod;


void scaleCopySurface(ScalingMethod method, SDL_Surface *srcSurface, SDL_Surface *desSurface);

#endif

