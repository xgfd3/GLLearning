//
// Created by xucz on 2020/8/10.
//

#ifndef ANDROIDOPENGL3DEMO_STENCILTESTING_H
#define ANDROIDOPENGL3DEMO_STENCILTESTING_H

#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
extern "C" {
#endif

#include <esUtil.h>
#include <ImageUtils.h>


void StencilTestingInit(ESContext *esContext);

void StencilTestingUnInit(ESContext *esContext);

void StencilTestingDraw(ESContext *esContext);

void StencilTestingUpdateTransformMatrix(ESContext *esContext, GLfloat rotateX, GLfloat rotateY,
                                       GLfloat scaleX, GLfloat scaleY);



#ifdef __cplusplus
}
#endif

#endif //ANDROIDOPENGL3DEMO_STENCILTESTING_H
