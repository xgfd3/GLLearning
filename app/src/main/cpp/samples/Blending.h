//
// Created by xucz on 2020/8/13.
//

#ifndef ANDROIDOPENGL3DEMO_BLENDING_H
#define ANDROIDOPENGL3DEMO_BLENDING_H

#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
extern "C" {
#endif

#include "../common/ImageUtils.h"
#include "../common/esUtil.h"

    void BlendingInit(ESContext *esContext);

    void BlendingUnInit(ESContext *esContext);

    void BlendingDraw(ESContext *esContext);



#ifdef __cplusplus
}
#endif

#endif //ANDROIDOPENGL3DEMO_BLENDING_H
