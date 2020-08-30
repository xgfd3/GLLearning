//
// Created by xucz on 2020/8/30.
//

#ifndef ANDROIDOPENGL3DEMO_LUTFILTER_H
#define ANDROIDOPENGL3DEMO_LUTFILTER_H
#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
extern "C" {
#endif

#include "../common/ImageUtils.h"
#include "../common/esUtil.h"
#include "CameraBase.h"


void LUTFilterInit(ESContext *esContext);

void LUTFilterUnInit(ESContext *esContext);

void LUTFilterSetCameraTexId(ESContext *esContext, GLuint texId, int texType, int width, int height, int facing);

void LUTFilterDraw(ESContext *esContext);


#ifdef __cplusplus
}
#endif
#endif //ANDROIDOPENGL3DEMO_LUTFILTER_H
