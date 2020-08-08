//
// Created by xucz on 2020/8/2.
//

#ifndef ANDROIDOPENGL3DEMO_EGL_H
#define ANDROIDOPENGL3DEMO_EGL_H

#include "../common/esUtil.h"
#include "../common/ImageUtils.h"

void EGLInit(ESContext *esContext);
void EGLUninit(ESContext *esContext);

void EGLDrawNormal(ESContext *esContext);
void EGLDrawMosaic(ESContext *esContext);
void EGLDrawGrid(ESContext *esContext);
void EGLDrawRotate(ESContext *esContext);
void EGLDrawEdge(ESContext *esContext);
void EGLDrawEnLarge(ESContext *esContext);
void EGLDrawUnKnow(ESContext *esContext);
void EGLDrawDeformation(ESContext *esContext);



#endif //ANDROIDOPENGL3DEMO_EGL_H
