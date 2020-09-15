//
// Created by xucz on 2020/9/15.
//

#include "RotateCircle.h"

extern "C" {

typedef struct __UserData {
    UserDataBase parent;

    GLuint s_TexSamplerLoc;
    GLuint u_offsetLoc;
    GLuint texSizeLoc;

    glm::vec2 texSize;

} UserData;

const char f2DShaderStr[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_TexSampler;\n"
        "uniform float u_offset;\n"
        "uniform vec2 texSize;\n"
        "const float PI = 3.141592653;\n"
        "void main(){ \n"
        "    vec2 imgTex = v_texCoord * texSize;\n"
        "    float r = 0.3 * texSize.x;\n"
        "    if(distance(imgTex, vec2(texSize.x / 2.0, texSize.y / 2.0)) < r){\n"
        "        vec2 tranTex = v_texCoord - 0.5;\n"
        "        vec2 imgTranTex = tranTex * texSize;\n"
        "        float len = length(imgTranTex);\n"
        "        float angle = 0.0;\n"
        "        angle = acos(imgTranTex.x / len);\n"
        "        if(tranTex.y < 0.0){\n"
        "             angle *= -1.0;\n"
        "        }\n"
        "        angle -= u_offset;\n"
        "        imgTranTex.x = len * cos(angle);\n"
        "        imgTranTex.y = len * sin(angle);\n"
        "        vec2 newTexCoords = imgTranTex / texSize + 0.5;\n"
        "        outColor = texture(s_TexSampler, newTexCoords);\n"
        "    } else { \n"
        "        outColor = texture(s_TexSampler, v_texCoord);\n"
        "    }\n"
        "}\n";

const char fOESShaderStr[] =
        "#version 300 es\n"
        "#extension GL_OES_EGL_image_external_essl3 : require\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform samplerExternalOES s_TexSampler;\n"
        "uniform float u_offset;\n"
        "uniform vec2 texSize;\n"
        "const float PI = 3.141592653;\n"
        "void main(){ \n"
        "    vec2 imgTex = v_texCoord * texSize;\n"
        "    float r = 0.3 * texSize.x;\n"
        "    if(distance(imgTex, vec2(texSize.x / 2.0, texSize.y / 2.0)) < r){\n"
        "        vec2 tranTex = v_texCoord - 0.5;\n"
        "        vec2 imgTranTex = tranTex * texSize;\n"
        "        float len = length(imgTranTex);\n"
        "        float angle = 0.0;\n"
        "        angle = acos(imgTranTex.x / len);\n"
        "        if(tranTex.y < 0.0){\n"
        "             angle *= -1.0;\n"
        "        }\n"
        "        angle -= u_offset;\n"
        "        imgTranTex.x = len * cos(angle);\n"
        "        imgTranTex.y = len * sin(angle);\n"
        "        vec2 newTexCoords = imgTranTex / texSize + 0.5;\n"
        "        outColor = texture(s_TexSampler, newTexCoords);\n"
        "    } else { \n"
        "        outColor = texture(s_TexSampler, v_texCoord);\n"
        "    }\n"
        "}\n";


void RotateCircleInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData) {
        userData = new UserData();
        esContext->userData = userData;
    }

    // 编译程序
    UserDataBase *pUserDataBase = (UserDataBase *) userData;
    char *fShaderStr;
    if (pUserDataBase->cameraTexType == TEX_TYPE_OES) {
        fShaderStr = (char *) fOESShaderStr;
    } else {
        fShaderStr = (char *) f2DShaderStr;
    }

    GLuint program = CameraBaseLoadProgram(pUserDataBase, fShaderStr);
    if (!program) {
        return;
    }
    userData->s_TexSamplerLoc = glGetUniformLocation(program, "s_TexSampler");
    userData->u_offsetLoc = glGetUniformLocation(program, "u_offset");
    userData->texSizeLoc = glGetUniformLocation(program, "texSize");
}

void RotateCircleUnInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData) {
        return;
    }
    CameraBaseRelease((UserDataBase *) userData);

    delete userData;
    esContext->userData = nullptr;
}

void RotateCircleSetCameraTexId(ESContext *esContext,
                                GLuint texId, int texType,
                                int width, int height,
                                int facing) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData) {
        return;
    }
    int shouldUpdate = CameraBaseUpdateParam((UserDataBase *) userData, texId, texType, width,
                                             height, facing);
    if (shouldUpdate) {
        CameraBaseUpdateMatrix((UserDataBase *) userData, esContext->width, esContext->height);
        userData->texSize = glm::vec2(1.0f * width, 1.0f * height);
        RotateCircleInit(esContext);
    }
}

void RotateCircleDraw(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData) {
        return;
    }

    UserDataBase *pUserDataBase = (UserDataBase *) userData;
    CameraBaseBeforeDraw(pUserDataBase, esContext->width, esContext->height);
    if (!pUserDataBase->cameraTexId) {
        return;
    }

    if (pUserDataBase->cameraTexType == TEX_TYPE_OES) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, pUserDataBase->cameraTexId);
        glUniform1i(userData->s_TexSamplerLoc, 0);
    } else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pUserDataBase->cameraTexId);
        glUniform1i(userData->s_TexSamplerLoc, 0);
    }

    // 图片坐标
    glUniform2fv(userData->texSizeLoc, 1, &userData->texSize[0]);

    // 偏移
    float offset = CameraBaseGetProgress(pUserDataBase, 30, 0, 0, 0xFFFFFF);
    glUniform1f(userData->u_offsetLoc, offset);


    CameraBaseAfterDraw(pUserDataBase);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, GL_NONE);
}

}