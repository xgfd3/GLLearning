//
// Created by xucz on 2020/8/13.
//

#ifndef ANDROIDOPENGL3DEMO_INSTANCING_H
#define ANDROIDOPENGL3DEMO_INSTANCING_H

#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
extern "C" {
#endif

#include "../common/ImageUtils.h"
#include "../common/esUtil.h"

void InstancingInit(ESContext *esContext);

void InstancingUnInit(ESContext *esContext);

void InstancingDraw(ESContext *esContext);

void InstancingUpdateTransformMatrix(ESContext *esContext, GLfloat angleX, GLfloat angleY, GLfloat scaleX, GLfloat scaleY);


#ifdef __cplusplus
}
#endif
#endif //ANDROIDOPENGL3DEMO_INSTANCING_H
