//
// Created by xucz on 2020/8/23.
//

#ifndef ANDROIDOPENGL3DEMO_GL_CAMERA_RENDER_JNI_H
#define ANDROIDOPENGL3DEMO_GL_CAMERA_JNI_H

#ifdef __cplusplus
extern "C" {
#endif
#include "common/esUtil.h"
#include "common/ImageUtils.h"



typedef struct __GLCamCapContext{
    ESContext esContext;

    // oes纹理id
    GLuint oesTextureId;

    // fbo
    GLuint fboId;
    GLuint fboTextureId;
    int fboWidth;
    int fboHeight;

    // 将oes纹理绘制到fbo上的程序
    GLuint program;
    GLuint textureLoc;
    GLuint vboIds[2];
    GLuint vaoId;

    // 输出纹理类型
    int outTexType;

} GLCameCapContext;




#ifdef __cplusplus
}
#endif
#endif //ANDROIDOPENGL3DEMO_GL_CAMERA_RENDER_JNI_H
