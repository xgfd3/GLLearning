//
// Created by xucz on 2020/8/9.
//

#ifndef ANDROIDOPENGL3DEMO_DEPTHTESTING_H
#define ANDROIDOPENGL3DEMO_DEPTHTESTING_H


#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
extern "C" {
#endif
#include <esUtil.h>
#include <ImageUtils.h>

void DepthTestingInit(ESContext *esContext);

void DepthTestingUnInit(ESContext *esContext);

void DepthTestingDraw(ESContext *esContext);

void DepthTestingUpdateTransformMatrix(ESContext *esContext, GLfloat rotateX, GLfloat rotateY,
                                        GLfloat scaleX, GLfloat scaleY);


#ifdef __cplusplus
}
#endif

#endif //ANDROIDOPENGL3DEMO_DEPTHTESTING_H
