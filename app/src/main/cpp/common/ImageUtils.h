//
// Created by xucz on 2020/7/26.
//

#ifndef ANDROIDOPENGL3DEMO_IMAGEUTILS_H
#define ANDROIDOPENGL3DEMO_IMAGEUTILS_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define IMAGE_FORMAT_RGBA           0x01
#define IMAGE_FORMAT_NV21           0x02
#define IMAGE_FORMAT_NV12           0x03
#define IMAGE_FORMAT_I420           0x04

typedef struct __NativeImage {
    int format;
    int width;
    int height;
    uint8_t *ppPlane[3];
} NativeImage;

void genNativeImage(NativeImage *pImage, int format, int width, int height, uint8_t *pData);

void allocNativeImage(NativeImage *pImage);

void freeNativeImage(NativeImage *pImage);

void copyNativeImage(NativeImage *pSrcImg, NativeImage *pDesImg);

#ifdef __cplusplus
}
#endif
#endif //ANDROIDOPENGL3DEMO_IMAGEUTILS_H
