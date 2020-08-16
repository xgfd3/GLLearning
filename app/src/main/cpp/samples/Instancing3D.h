//
// Created by xucz on 2020/8/14.
//

#ifndef ANDROIDOPENGL3DEMO_INSTANCING3D_H
#define ANDROIDOPENGL3DEMO_INSTANCING3D_H

#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
extern "C" {
#endif

#include "../common/ImageUtils.h"
#include "../common/esUtil.h"

void Instancing3DInit(ESContext *esContext);

void Instancing3DUnInit(ESContext *esContext);

void Instancing3DDraw(ESContext *esContext);

void Instancing3DUpdateTransformMatrix(ESContext *esContext, GLfloat angleX, GLfloat angleY, GLfloat scaleX, GLfloat scaleY);




#ifdef __cplusplus
}
#endif

#endif //ANDROIDOPENGL3DEMO_INSTANCING3D_H
