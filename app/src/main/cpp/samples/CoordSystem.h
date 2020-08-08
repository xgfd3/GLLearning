//
// Created by xucz on 2020/8/8.
//

#ifndef ANDROIDOPENGL3DEMO_COORDSYSTEM_H
#define ANDROIDOPENGL3DEMO_COORDSYSTEM_H


#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
extern "C" {
#endif

#include "../common/ImageUtils.h"
#include "../common/esUtil.h"

void CoorderSystemInit(ESContext *context);

void CoorderSystemUnInit(ESContext *context);

void CoorderSystemDraw(ESContext *context);

void CoorderSystemUpdateTransformMatrix(ESContext *context, GLfloat rotateX, GLfloat rotateY, GLfloat scaleX, GLfloat scalY);

#ifdef __cplusplus
}
#endif
#endif //ANDROIDOPENGL3DEMO_COORDSYSTEM_H
