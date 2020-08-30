

#ifndef GLAPI_NATIVE_H
#define GLAPI_NATIVE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "common/esUtil.h"
#include "common/ImageUtils.h"


#include "samples/CoordSystem.h"
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
#include "samples/Blending.h"
#include "samples/Instancing.h"
#include "samples/Instancing3D.h"
#include "samples/Particles.h"
#include "samples/SkyBox.h"

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
#define WHAT_DRAW_BLENDING              WHAT_DRAW_BASE + 12
#define WHAT_DRAW_INSTANCING            WHAT_DRAW_BASE + 13
#define WHAT_DRAW_INSTANCING3D          WHAT_DRAW_BASE + 14
#define WHAT_DRAW_PARTICLES             WHAT_DRAW_BASE + 15
#define WHAT_DRAW_SKYBOX                WHAT_DRAW_BASE + 16


#define WHAT_DRAW_EGL 0x00001000
#define WHAT_DRAW_EGL_NORMAL            WHAT_DRAW_EGL + 1
#define WHAT_DRAW_EGL_MOSAIC            WHAT_DRAW_EGL + 2
#define WHAT_DRAW_EGL_GRID              WHAT_DRAW_EGL + 3
#define WHAT_DRAW_EGL_ROTATE            WHAT_DRAW_EGL + 4
#define WHAT_DRAW_EGL_EDGE              WHAT_DRAW_EGL + 5
#define WHAT_DRAW_EGL_ENLARGE           WHAT_DRAW_EGL + 6
#define WHAT_DRAW_EGL_UNKNOW            WHAT_DRAW_EGL + 7
#define WHAT_DRAW_EGL_DEFORMATION       WHAT_DRAW_EGL + 8


typedef struct __GLSampleContext {
    // 子结构体里定义一个父结构体变量，必须放在子结构体里的第一位
    ESContext esContext;

    // Callbacks what
    int cb_what;

    /// Callbacks
    void (  *initFunc )(struct __ESContext *);

    void (  *drawFunc )(struct __ESContext *);

    void (  *uninitFunc )(struct __ESContext *);

    void (  *updateTouchLoc )(struct __ESContext *, GLfloat, GLfloat);

    void (  *updateTransformMatrix )(struct __ESContext *, GLfloat, GLfloat, GLfloat, GLfloat);

    void ( *loadMultiImageWithIndex)(struct __ESContext *, int , int, int, int, uint8_t *);

} GLSampleContext;


void findAndFillDrawMethods(GLSampleContext *esContext, int what) {
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
        case WHAT_DRAW_BLENDING:
            esContext->initFunc = (void (*)(ESContext *)) BlendingInit;
            esContext->drawFunc = (void (*)(ESContext *)) BlendingDraw;
            esContext->uninitFunc = (void (*)(ESContext *)) BlendingUnInit;
            break;
        case WHAT_DRAW_INSTANCING:
            esContext->initFunc = (void (*)(ESContext *)) InstancingInit;
            esContext->drawFunc = (void (*)(ESContext *)) InstancingDraw;
            esContext->uninitFunc = (void (*)(ESContext *)) InstancingUnInit;
            break;
        case WHAT_DRAW_INSTANCING3D:
            esContext->initFunc = (void (*)(ESContext *)) Instancing3DInit;
            esContext->drawFunc = (void (*)(ESContext *)) Instancing3DDraw;
            esContext->uninitFunc = (void (*)(ESContext *)) Instancing3DUnInit;
            esContext->updateTransformMatrix = (void (*)(ESContext *, GLfloat, GLfloat, GLfloat,
                                                         GLfloat)) Instancing3DUpdateTransformMatrix;
            break;
        case WHAT_DRAW_PARTICLES:
            esContext->initFunc = (void (*)(ESContext *)) ParticlesInit;
            esContext->drawFunc = (void (*)(ESContext *)) ParticlesDraw;
            esContext->uninitFunc = (void (*)(ESContext *)) ParticlesUnInit;
            esContext->updateTransformMatrix = (void (*)(ESContext *, GLfloat, GLfloat, GLfloat,
                                                         GLfloat)) ParticlesUpdateTransformMatrix;
            break;
        case WHAT_DRAW_SKYBOX:
            esContext->initFunc = (void (*)(ESContext *)) SkyBoxInit;
            esContext->drawFunc = (void (*)(ESContext *)) SkyBoxDraw;
            esContext->uninitFunc = (void (*)(ESContext *)) SkyBoxUnInit;
            esContext->updateTransformMatrix = (void (*)(ESContext *, GLfloat, GLfloat, GLfloat,
                                                         GLfloat)) SkyBoxUpdateTransformMatrix;
            break;
    }
}


#ifdef __cplusplus
}
#endif
#endif