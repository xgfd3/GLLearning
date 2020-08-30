//
// Created by xucz on 2020/8/30.
//

#ifndef ANDROIDOPENGL3DEMO_SPLITSCREEN_H
#define ANDROIDOPENGL3DEMO_SPLITSCREEN_H
#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
extern "C" {
#endif

#include "../common/ImageUtils.h"
#include "../common/esUtil.h"
#include "CameraBase.h"

void SplitScreenInit(ESContext *esContext);

void SplitScreenUnInit(ESContext *esContext);

void SplitScreenSetCameraTexId(ESContext *esContext, GLuint texId, int texType, int width, int height, int facing);

void SplitScreenDraw(ESContext *esContext);


#ifdef __cplusplus
}
#endif
#endif //ANDROIDOPENGL3DEMO_SPLITSCREEN_H
