

#ifndef GLAPI_NATIVE_H
#define GLAPI_NATIVE_H

#include "common/esUtil.h"
#include "1_shape/Triangle.h"
#include "2_VBO&VAO/VAO.h"
#include "2_VBO&VAO/VBO.h"

#define WHAT_DRAW_BASE 100
#define WHAT_DRAW_TRIANGLE  WHAT_DRAW_BASE+1
#define WHAT_DRAW_VBO1       WHAT_DRAW_BASE+2
#define WHAT_DRAW_VBO2       WHAT_DRAW_BASE+3
#define WHAT_DRAW_VAO       WHAT_DRAW_BASE+4




void findAndFillDrawMethods(ESContext *esContext, int what){
    switch (what){
        case WHAT_DRAW_TRIANGLE:
            esContext->initFunc = (void(*)(struct ESContext *))TriangleInit;
            esContext->drawFunc = (void(*)(struct ESContext *))TriangleDraw;
            esContext->uninitFunc = (void(*)(struct ESContext *))TriangleShutdown;
            break;
        case WHAT_DRAW_VBO1:
            esContext->initFunc = (void(*)(struct ESContext *))VBOInit;
            esContext->drawFunc = (void(*)(struct ESContext *))VBODraw1;
            esContext->uninitFunc = (void(*)(struct ESContext *))VBOShutdown;
            break;
        case WHAT_DRAW_VBO2:
            esContext->initFunc = (void(*)(struct ESContext *))VBOInit;
            esContext->drawFunc = (void(*)(struct ESContext *))VBODraw2;
            esContext->uninitFunc = (void(*)(struct ESContext *))VBOShutdown;
            break;
        case WHAT_DRAW_VAO:
            esContext->initFunc = (void(*)(struct ESContext *))VAOInitialize;
            esContext->drawFunc = (void(*)(struct ESContext *))VAODraw;
            esContext->uninitFunc = (void(*)(struct ESContext *))VAOuninit;
            break;

    }
}


#endif