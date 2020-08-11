//
// Created by xucz on 2020/8/9.
//

#ifndef ANDROIDOPENGL3DEMO_BASICLIGHTING_H
#define ANDROIDOPENGL3DEMO_BASICLIGHTING_H


#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
extern "C" {
#endif

#include "../common/ImageUtils.h"
#include "../common/esUtil.h"

    void BasicLightingInit(ESContext *esContext);

    void BasicLightingUnInit(ESContext *esContext);

    void BasicLightingDraw(ESContext *esContext);

    void BasicLightingUpdateTransformMatrix(ESContext *esContext, GLfloat rotateX, GLfloat rotateY,
            GLfloat scaleX, GLfloat scaleY);



#ifdef __cplusplus
}
#endif
#endif //ANDROIDOPENGL3DEMO_BASICLIGHTING_H
