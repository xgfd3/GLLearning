//
// Created by xucz on 2019-06-22.
//

#ifndef ANDROIDOPENGL3DEMO_TRIANGLE_H
#define ANDROIDOPENGL3DEMO_TRIANGLE_H

#include "../common/esUtil.h"

typedef struct {
    GLuint programObject;
} UserData;

int TriangleInit(struct ESContext *esContext);

void TriangleDraw(struct ESContext *esContext);

void TriangleShutdown(struct ESContext *esContext);


#endif //ANDROIDOPENGL3DEMO_TRIANGLE_H
