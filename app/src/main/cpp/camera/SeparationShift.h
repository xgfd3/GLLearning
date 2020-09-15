//
// Created by xucz on 2020/9/7.
// 分色偏移


#ifndef ANDROIDOPENGL3DEMO_SEPARATIONSHIFT_H
#define ANDROIDOPENGL3DEMO_SEPARATIONSHIFT_H

#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
extern "C" {
#endif

#include "../common/ImageUtils.h"
#include "../common/esUtil.h"
#include "CameraBase.h"

void SeparationShiftInit(ESContext *esContext);

void SeparationShiftUnInit(ESContext *esContext);

void SeparationShiftSetCameraTexId(ESContext *esContext, GLuint texId, int texType, int width, int height, int facing);

void SeparationShiftDraw(ESContext *esContext);



#ifdef __cplusplus
}
#endif
#endif //ANDROIDOPENGL3DEMO_SEPARATIONSHIFT_H
