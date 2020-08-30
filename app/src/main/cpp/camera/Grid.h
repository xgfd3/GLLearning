//
// Created by xucz on 2020/8/30.
//

#ifndef ANDROIDOPENGL3DEMO_GRID_H
#define ANDROIDOPENGL3DEMO_GRID_H
#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
extern "C" {
#endif

#include "CameraBase.h"

void GridInit(ESContext *esContext);

void GridUnInit(ESContext *esContext);

void GridSetCameraTexId(ESContext *esContext, GLuint texId, int texType, int width, int height, int facing);

void GridDraw(ESContext *esContext);


#ifdef __cplusplus
}
#endif
#endif //ANDROIDOPENGL3DEMO_GRID_H
