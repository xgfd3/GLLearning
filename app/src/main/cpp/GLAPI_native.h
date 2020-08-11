

#ifndef GLAPI_NATIVE_H
#define GLAPI_NATIVE_H

#include "common/esUtil.h"
#include "common/ImageUtils.h"


#include "samples/CoordSystem.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "samples/Triangle.h"
#include "samples/VAO.h"
#include "samples/VBO.h"
#include "samples/TextureMap.h"
#include "samples/YUVTextureMap.h"
#include "samples/FBO.h"
#include "samples/EGL.h"
#include "samples/TransformFeedback.h"
#include "samples/BasicLighting.h"
#include "samples/DepthTesting.h"
#include "samples/StencilTesting.h"

#define WHAT_DRAW_BASE 0x00000010
#define WHAT_DRAW_TRIANGLE              WHAT_DRAW_BASE + 1
#define WHAT_DRAW_TEXTUREMAP            WHAT_DRAW_BASE + 2
#define WHAT_DRAW_YUVTEXTUREMAP         WHAT_DRAW_BASE + 3
#define WHAT_DRAW_VBO                   WHAT_DRAW_BASE + 4
#define WHAT_DRAW_VAO                   WHAT_DRAW_BASE + 5
#define WHAT_DRAW_FBO                   WHAT_DRAW_BASE + 6
#define WHAT_DRAW_TRANSFORM_FEEDBACK    WHAT_DRAW_BASE + 7
#define WHAT_DRAW_COORDER_SYSTEM        WHAT_DRAW_BASE + 8
#define WHAT_DRAW_BASIC_LIGHTING        WHAT_DRAW_BASE + 9
#define WHAT_DRAW_DEPTH_TESTING         WHAT_DRAW_BASE + 10
#define WHAT_DRAW_STENCIL_TESTING       WHAT_DRAW_BASE + 11


#define WHAT_DRAW_EGL 0x00001000
#define WHAT_DRAW_EGL_NORMAL            WHAT_DRAW_EGL + 1
#define WHAT_DRAW_EGL_MOSAIC            WHAT_DRAW_EGL + 2
#define WHAT_DRAW_EGL_GRID              WHAT_DRAW_EGL + 3
#define WHAT_DRAW_EGL_ROTATE            WHAT_DRAW_EGL + 4
#define WHAT_DRAW_EGL_EDGE              WHAT_DRAW_EGL + 5
#define WHAT_DRAW_EGL_ENLARGE           WHAT_DRAW_EGL + 6
#define WHAT_DRAW_EGL_UNKNOW            WHAT_DRAW_EGL + 7
#define WHAT_DRAW_EGL_DEFORMATION       WHAT_DRAW_EGL + 8


void findAndFillDrawMethods(ESContext *esContext, int what) {
    esContext->updateTouchLoc = 0;
    esContext->updateTransformMatrix = 0;

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
        case WHAT_DRAW_EGL_NORMAL     :
            esContext->initFunc = (void (*)(ESContext *)) EGLInit;
            esContext->drawFunc = (void (*)(ESContext *)) EGLDrawNormal;
            esContext->uninitFunc = (void (*)(ESContext *)) EGLUninit;
            break;
        case WHAT_DRAW_EGL_MOSAIC     :
            esContext->initFunc = (void (*)(ESContext *)) EGLInit;
            esContext->drawFunc = (void (*)(ESContext *)) EGLDrawMosaic;
            esContext->uninitFunc = (void (*)(ESContext *)) EGLUninit;
            break;
        case WHAT_DRAW_EGL_GRID       :
            esContext->initFunc = (void (*)(ESContext *)) EGLInit;
            esContext->drawFunc = (void (*)(ESContext *)) EGLDrawGrid;
            esContext->uninitFunc = (void (*)(ESContext *)) EGLUninit;
            break;
        case WHAT_DRAW_EGL_ROTATE     :
            esContext->initFunc = (void (*)(ESContext *)) EGLInit;
            esContext->drawFunc = (void (*)(ESContext *)) EGLDrawRotate;
            esContext->uninitFunc = (void (*)(ESContext *)) EGLUninit;
            break;
        case WHAT_DRAW_EGL_EDGE       :
            esContext->initFunc = (void (*)(ESContext *)) EGLInit;
            esContext->drawFunc = (void (*)(ESContext *)) EGLDrawEdge;
            esContext->uninitFunc = (void (*)(ESContext *)) EGLUninit;
            break;
        case WHAT_DRAW_EGL_ENLARGE    :
            esContext->initFunc = (void (*)(ESContext *)) EGLInit;
            esContext->drawFunc = (void (*)(ESContext *)) EGLDrawEnLarge;
            esContext->uninitFunc = (void (*)(ESContext *)) EGLUninit;
            break;
        case WHAT_DRAW_EGL_UNKNOW     :
            esContext->initFunc = (void (*)(ESContext *)) EGLInit;
            esContext->drawFunc = (void (*)(ESContext *)) EGLDrawUnKnow;
            esContext->uninitFunc = (void (*)(ESContext *)) EGLUninit;
            break;
        case WHAT_DRAW_EGL_DEFORMATION:
            esContext->initFunc = (void (*)(ESContext *)) EGLInit;
            esContext->drawFunc = (void (*)(ESContext *)) EGLDrawDeformation;
            esContext->uninitFunc = (void (*)(ESContext *)) EGLUninit;
            break;
        case WHAT_DRAW_TRANSFORM_FEEDBACK:
            esContext->initFunc = (void (*)(ESContext *)) TransformFeedbackInit;
            esContext->drawFunc = (void (*)(ESContext *)) TransformFeedbackDraw;
            esContext->uninitFunc = (void (*)(ESContext *)) TransformFeedbackUninit;
            break;
        case WHAT_DRAW_COORDER_SYSTEM:
            esContext->initFunc = (void (*)(ESContext *)) CoorderSystemInit;
            esContext->drawFunc = (void (*)(ESContext *)) CoorderSystemDraw;
            esContext->uninitFunc = (void (*)(ESContext *)) CoorderSystemUnInit;
            esContext->updateTransformMatrix = (void (*)(ESContext *, GLfloat, GLfloat, GLfloat,
                                                         GLfloat)) CoorderSystemUpdateTransformMatrix;
            break;
        case WHAT_DRAW_BASIC_LIGHTING:
            esContext->initFunc = (void (*)(ESContext *)) BasicLightingInit;
            esContext->drawFunc = (void (*)(ESContext *)) BasicLightingDraw;
            esContext->uninitFunc = (void (*)(ESContext *)) BasicLightingUnInit;
            esContext->updateTransformMatrix = (void (*)(ESContext *, GLfloat, GLfloat, GLfloat,
                                                         GLfloat)) BasicLightingUpdateTransformMatrix;
            break;
        case WHAT_DRAW_DEPTH_TESTING:
            esContext->initFunc = (void (*)(ESContext *)) DepthTestingInit;
            esContext->drawFunc = (void (*)(ESContext *)) DepthTestingDraw;
            esContext->uninitFunc = (void (*)(ESContext *)) DepthTestingUnInit;
            esContext->updateTransformMatrix = (void (*)(ESContext *, GLfloat, GLfloat, GLfloat,
                                                         GLfloat)) DepthTestingUpdateTransformMatrix;
            break;
        case WHAT_DRAW_STENCIL_TESTING:
            esContext->initFunc = (void (*)(ESContext *)) StencilTestingInit;
            esContext->drawFunc = (void (*)(ESContext *)) StencilTestingDraw;
            esContext->uninitFunc = (void (*)(ESContext *)) StencilTestingUnInit;
            esContext->updateTransformMatrix = (void (*)(ESContext *, GLfloat, GLfloat, GLfloat,
                                                         GLfloat)) StencilTestingUpdateTransformMatrix;
            break;

    }
}


#ifdef __cplusplus
}
#endif
#endif