//
// Created by xucz on 2020/7/26.
//

#include "ImageUtils.h"

void genNativeImage(NativeImage *pImage, int format, int width, int height, uint8_t *pData) {
    if (pImage == NULL) {
        return;
    }
    pImage->format = format;
    pImage->width = width;
    pImage->height = height;
    pImage->ppPlane[0] = pData;
    switch (format) {
        case IMAGE_FORMAT_NV12:
        case IMAGE_FORMAT_NV21:
            pImage->ppPlane[1] = pImage->ppPlane[0] + width * height;
            break;
        case IMAGE_FORMAT_I420:
            pImage->ppPlane[1] = pImage->ppPlane[0] + width * height;
            pImage->ppPlane[2] = pImage->ppPlane[1] + width * height / 4;
            break;
        default:
            break;
    }
}

void allocNativeImage(NativeImage *pImage) {
    if (pImage == NULL) {
        return;
    }
    if (pImage->width == 0 || pImage->height == 0) {
        return;
    }
    switch (pImage->format) {
        case IMAGE_FORMAT_RGBA: {
            pImage->ppPlane[0] = malloc(pImage->width * pImage->height * 4);
        }
            break;
        case IMAGE_FORMAT_NV12:
        case IMAGE_FORMAT_NV21: {
            pImage->ppPlane[0] = malloc(pImage->width * pImage->height * 1.5);
            pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height;
        }
            break;
        case IMAGE_FORMAT_I420: {
            pImage->ppPlane[0] = malloc(pImage->width * pImage->height * 1.5);
            pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height;
            pImage->ppPlane[2] = pImage->ppPlane[1] + pImage->width * (pImage->height >> 2);
        }
            break;
        default:
            break;
    }
}

void freeNativeImage(NativeImage *pImage) {
    if (pImage == NULL || pImage->ppPlane[0] == NULL) {
        return;
    }

    free(pImage->ppPlane[0]);
    pImage->ppPlane[0] = NULL;
    pImage->ppPlane[1] = NULL;
    pImage->ppPlane[1] = NULL;
}

void copyNativeImage(NativeImage *pSrcImg, NativeImage *pDstImg) {
    if (pSrcImg == NULL || pSrcImg->ppPlane[0] == NULL) {
        return;
    }

    pDstImg->format = pSrcImg->format;
    pDstImg->width = pSrcImg->width;
    pDstImg->height = pSrcImg->height;

    if (pDstImg->ppPlane[0] == NULL) {
        allocNativeImage(pDstImg);
    }

    switch (pSrcImg->format) {
        case IMAGE_FORMAT_I420:
        case IMAGE_FORMAT_NV21:
        case IMAGE_FORMAT_NV12:
            memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0],
                   pSrcImg->width * pSrcImg->height * 1.5);
            break;
        case IMAGE_FORMAT_RGBA:
            memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->width * pSrcImg->height * 4);
            break;
        default:
            break;
    }
}
