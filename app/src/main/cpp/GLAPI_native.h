

#ifndef GLAPI_NATIVE_H
#define GLAPI_NATIVE_H

#include "common/esUtil.h"
#include "common/ImageUtils.h"

#include "samples/Triangle.h"
#include "samples/VAO.h"
#include "samples/VBO.h"
#include "samples/TextureMap.h"
#include "samples/YUVTextureMap.h"
#include "samples/FBO.h"

#define WHAT_DRAW_BASE 100
#define WHAT_DRAW_TRIANGLE              WHAT_DRAW_BASE+1
#define WHAT_DRAW_TEXTUREMAP            WHAT_DRAW_BASE+2
#define WHAT_DRAW_YUVTEXTUREMAP         WHAT_DRAW_BASE+3
#define WHAT_DRAW_VBO                   WHAT_DRAW_BASE+4
#define WHAT_DRAW_VAO                   WHAT_DRAW_BASE+5
#define WHAT_DRAW_FBO                   WHAT_DRAW_BASE+6


void findAndFillDrawMethods(ESContext *esContext, int what) {
    switch (what) {
        case WHAT_DRAW_TRIANGLE:
            esContext->initFunc = (void (*)(ESContext *)) TriangleInit;
            esContext->drawFunc = (void (*)(ESContext *)) TriangleDraw;
            esContext->uninitFunc = (void (*)(ESContext *)) TriangleShutdown;
            break;
        case WHAT_DRAW_TEXTUREMAP:
            esContext->initFunc = (void (*)(ESContext *)) TextureMapInit;
            esContext->drawFunc = (void (*)(ESContext *)) TextureMapDraw;
            esContext->uninitFunc = (void (*)(ESContext *)) TextureMapUninit;
            break;
        case WHAT_DRAW_YUVTEXTUREMAP:
            esContext->initFunc = (void (*)(ESContext *)) YUVTextureMapInit;
            esContext->drawFunc = (void (*)(ESContext *)) YUVTextureMapDraw;
            esContext->uninitFunc = (void (*)(ESContext *)) YUVTextureMapUnInit;
            break;
        case WHAT_DRAW_VBO:
            esContext->initFunc = (void (*)(ESContext *)) VBOInit;
            esContext->drawFunc = (void (*)(ESContext *)) VBODraw;
            esContext->uninitFunc = (void (*)(ESContext *)) VBOShutdown;
            break;
            break;
        case WHAT_DRAW_VAO:
            esContext->initFunc = (void (*)(ESContext *)) VAOInitialize;
            esContext->drawFunc = (void (*)(ESContext *)) VAODraw;
            esContext->uninitFunc = (void (*)(ESContext *)) VAOuninit;
            break;
        case WHAT_DRAW_FBO:
            esContext->initFunc = (void (*)(ESContext *)) FBOInit;
            esContext->drawFunc = (void (*)(ESContext *)) FBODraw;
            esContext->uninitFunc = (void (*)(ESContext *)) FBOUninit;
            break;
    }
}


#endif