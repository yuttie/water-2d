#ifndef __IMGSCALE_H
#define __IMGSCALE_H


/* Îóµó·¿ */
enum ScalingMethod
{
    SM_SIMPLE,
    SM_BI_LINEAR
};


void ScaleCopySurface(enum ScalingMethod method, SDL_Surface *srcSurface, SDL_Surface *desSurface);

#endif

