//
// Created by xucz on 2020/8/16.
//

#ifndef ANDROIDOPENGL3DEMO_PARTICLES_H
#define ANDROIDOPENGL3DEMO_PARTICLES_H
#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
#include <ctime>
#include <cstdlib>
#include <algorithm>
extern "C" {
#endif
#include "../common/ImageUtils.h"
#include "../common/esUtil.h"

void ParticlesInit(ESContext *esContext);

void ParticlesUnInit(ESContext *esContext);

void ParticlesDraw(ESContext *esContext);

void ParticlesUpdateTransformMatrix(ESContext *esContext, GLfloat rotateX, GLfloat rotateY,
                                        GLfloat scaleX, GLfloat scaleY);


#ifdef __cplusplus
}
#endif
#endif //ANDROIDOPENGL3DEMO_PARTICLES_H
