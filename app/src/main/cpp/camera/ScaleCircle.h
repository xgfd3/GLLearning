//
// Created by xucz on 2020/8/29.
//

#ifndef ANDROIDOPENGL3DEMO_SCALE_CIRCLE_H
#define ANDROIDOPENGL3DEMO_SCALE_CIRCLE_H

#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
extern "C" {
#endif

#include "../common/ImageUtils.h"
#include "../common/esUtil.h"
#include "CameraBase.h"

void ScaleCircleInit(ESContext *esContext);

void ScaleCircleUnInit(ESContext *esContext);

void ScaleCircleSetCameraTexId(ESContext *esContext, GLuint texId, int texType, int width, int height, int facing);

void ScaleCircleDraw(ESContext *esContext);


#ifdef __cplusplus
}
#endif
#endif //ANDROIDOPENGL3DEMO_NORMAL_H
