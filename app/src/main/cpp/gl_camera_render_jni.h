//
// Created by xucz on 2020/8/23.
//

#ifndef ANDROIDOPENGL3DEMO_GL_CAMERA_JNI_H
#define ANDROIDOPENGL3DEMO_GL_CAMERA_RENDER_JNI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/esUtil.h"
#include "common/ImageUtils.h"
#include "camera/Normal.h"
#include "camera/Grid.h"
#include "camera/SplitScreen.h"
#include "camera/ScaleCircle.h"
#include "camera/LUTFilter.h"
#include "camera/SeparationShift.h"
#include "camera/SoulOut.h"
#include "camera/RotateCircle.h"
#include "camera/PictInPict.h"

#define WHAT_DRAW_CAMERA                    0x00000100
#define WHAT_DRAW_CAMERA_NORMAL             WHAT_DRAW_CAMERA + 1
#define WHAT_DRAW_CAMERA_GRID               WHAT_DRAW_CAMERA + 2
#define WHAT_DRAW_CAMERA_SPLIT_SCREEN       WHAT_DRAW_CAMERA + 3
#define WHAT_DRAW_CAMERA_SCALE_CIRCLE       WHAT_DRAW_CAMERA + 4
#define WHAT_DRAW_CAMERA_LUT_FILTER_1       WHAT_DRAW_CAMERA + 5
#define WHAT_DRAW_CAMERA_LUT_FILTER_2       WHAT_DRAW_CAMERA + 6
#define WHAT_DRAW_CAMERA_LUT_FILTER_3       WHAT_DRAW_CAMERA + 7
#define WHAT_DRAW_CAMERA_LUT_FILTER_4       WHAT_DRAW_CAMERA + 8
#define WHAT_DRAW_CAMERA_LUT_FILTER_5       WHAT_DRAW_CAMERA + 9
#define WHAT_DRAW_CAMERA_LUT_FILTER_6       WHAT_DRAW_CAMERA + 10
#define WHAT_DRAW_CAMERA_SEPARATION_SHIFT   WHAT_DRAW_CAMERA + 11
#define WHAT_DRAW_CAMERA_SOUL_OUT           WHAT_DRAW_CAMERA + 12
#define WHAT_DRAW_CAMERA_ROTATE_CIRCLE      WHAT_DRAW_CAMERA + 13
#define WHAT_DRAW_CAMERA_PICT_IN_PICT       WHAT_DRAW_CAMERA + 14

typedef struct __GLCameraContext {

    ESContext esContext;

    // 相机纹理数据
    GLuint cameraTexId;
    int cameraTexType;
    int cameraTexWidth;
    int cameraTexHeight;
    int cameraFacing;

    // Callbacks what
    int cb_what;

    /// Callbacks
    void (  *initFunc )(struct __ESContext *);

    void (  *drawFunc )(struct __ESContext *);

    void (  *uninitFunc )(struct __ESContext *);

    void (  *setCameraTexId )(struct __ESContext *, GLuint, int, int, int, int);

} GLCameraContext;


void findAndFillDrawCameraMethods(GLCameraContext *esContext, int what) {
    esContext->initFunc = 0;
    esContext->drawFunc = 0;
    esContext->uninitFunc = 0;
    esContext->setCameraTexId = 0;

    switch (what) {
        case WHAT_DRAW_CAMERA_NORMAL:
            esContext->initFunc = NormalInit;
            esContext->drawFunc = NormalDraw;
            esContext->uninitFunc = NormalUnInit;
            esContext->setCameraTexId = NormalSetCameraTexId;
            break;
        case WHAT_DRAW_CAMERA_GRID:
            esContext->initFunc = GridInit;
            esContext->drawFunc = GridDraw;
            esContext->uninitFunc = GridUnInit;
            esContext->setCameraTexId = GridSetCameraTexId;
            break;
        case WHAT_DRAW_CAMERA_SPLIT_SCREEN:
            esContext->initFunc = SplitScreenInit;
            esContext->drawFunc = SplitScreenDraw;
            esContext->uninitFunc = SplitScreenUnInit;
            esContext->setCameraTexId = SplitScreenSetCameraTexId;
            break;
        case WHAT_DRAW_CAMERA_SCALE_CIRCLE:
            esContext->initFunc = ScaleCircleInit;
            esContext->drawFunc = ScaleCircleDraw;
            esContext->uninitFunc = ScaleCircleUnInit;
            esContext->setCameraTexId = ScaleCircleSetCameraTexId;
            break;
        case WHAT_DRAW_CAMERA_LUT_FILTER_1:
        case WHAT_DRAW_CAMERA_LUT_FILTER_2:
        case WHAT_DRAW_CAMERA_LUT_FILTER_3:
        case WHAT_DRAW_CAMERA_LUT_FILTER_4:
        case WHAT_DRAW_CAMERA_LUT_FILTER_5:
        case WHAT_DRAW_CAMERA_LUT_FILTER_6:
            esContext->initFunc = LUTFilterInit;
            esContext->drawFunc = LUTFilterDraw;
            esContext->uninitFunc = LUTFilterUnInit;
            esContext->setCameraTexId = LUTFilterSetCameraTexId;
            break;
        case WHAT_DRAW_CAMERA_SEPARATION_SHIFT:
            esContext->initFunc = SeparationShiftInit;
            esContext->drawFunc = SeparationShiftDraw;
            esContext->uninitFunc = SeparationShiftUnInit;
            esContext->setCameraTexId = SeparationShiftSetCameraTexId;
            break;
        case WHAT_DRAW_CAMERA_SOUL_OUT:
            esContext->initFunc = SoulOutInit;
            esContext->drawFunc = SoulOutDraw;
            esContext->uninitFunc = SoulOutUnInit;
            esContext->setCameraTexId = SoulOutSetCameraTexId;
            break;
        case WHAT_DRAW_CAMERA_ROTATE_CIRCLE:
            esContext->initFunc = RotateCircleInit;
            esContext->drawFunc = RotateCircleDraw;
            esContext->uninitFunc = RotateCircleUnInit;
            esContext->setCameraTexId = RotateCircleSetCameraTexId;
            break;
        case WHAT_DRAW_CAMERA_PICT_IN_PICT:
            esContext->initFunc = PictInPictInit;
            esContext->drawFunc = PictInPictDraw;
            esContext->uninitFunc = PictInPictUnInit;
            esContext->setCameraTexId = PictInPictSetCameraTexId;
            break;
    }
}

#ifdef __cplusplus
}
#endif
#endif //ANDROIDOPENGL3DEMO_GL_CAMERA_JNI_H
