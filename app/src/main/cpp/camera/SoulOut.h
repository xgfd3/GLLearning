//
// Created by xucz on 2020/9/15.
// 灵魂出窍

#ifndef ANDROIDOPENGL3DEMO_SOULOUT_H
#define ANDROIDOPENGL3DEMO_SOULOUT_H

#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
extern "C" {
#endif

#include "../common/ImageUtils.h"
#include "../common/esUtil.h"
#include "CameraBase.h"

void SoulOutInit(ESContext *esContext);

void SoulOutUnInit(ESContext *esContext);

void SoulOutSetCameraTexId(ESContext *esContext, GLuint texId, int texType, int width, int height, int facing);

void SoulOutDraw(ESContext *esContext);



#ifdef __cplusplus
}
#endif

#endif //ANDROIDOPENGL3DEMO_SOULOUT_H
