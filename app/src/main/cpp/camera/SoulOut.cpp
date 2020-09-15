//
// Created by xucz on 2020/9/15.
//

#include "SoulOut.h"

extern "C" {

typedef struct __UserData {
    UserDataBase parent;

    GLuint s_TexSamplerLoc;
    GLuint u_offsetLoc;

} UserData;

const char f2DShaderStr[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_TexSampler;\n"
        "uniform float u_offset;\n"
        "const float MAX_ALPHA = 0.5;\n"
        "const float MAX_SCALE = 0.8;\n"
        "void main(){ \n"
        //根据偏移量计算混合系数 alpha
        "    float alpha = MAX_ALPHA * (1.0 - u_offset);\n"
        // 根据偏移量计算混合系数 scale
        "    float scale = 1.0 + u_offset * MAX_SCALE;\n"
        // 缩放操作
        "    float scale_x = 0.5 + (v_texCoord.x - 0.5) / scale;\n"
        "    float scale_y = 0.5 + (v_texCoord.y - 0.5) / scale;\n"
        "    vec2 scaleCoord = vec2(scale_x, scale_y);\n"
        "    vec4 maskColor = texture(s_TexSampler, scaleCoord);\n"
        "    vec4 originColor = texture(s_TexSampler, v_texCoord);\n"
        // 加权混合
        "    outColor = originColor * (1.0 - alpha) + maskColor * alpha;\n"
        "}\n";

const char fOESShaderStr[] =
        "#version 300 es\n"
        "#extension GL_OES_EGL_image_external_essl3 : require\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform samplerExternalOES s_TexSampler;\n"
        "uniform float u_offset;\n"
        "const float MAX_ALPHA = 0.5;\n"
        "const float MAX_SCALE = 0.8;\n"
        "void main(){ \n"
        //根据偏移量计算混合系数 alpha
        "    float alpha = MAX_ALPHA * (1.0 - u_offset);\n"
        // 根据偏移量计算混合系数 scale
        "    float scale = 1.0 + u_offset * MAX_SCALE;\n"
        // 缩放操作
        "    float scale_x = 0.5 + (v_texCoord.x - 0.5) / scale;\n"
        "    float scale_y = 0.5 + (v_texCoord.y - 0.5) / scale;\n"
        "    vec2 scaleCoord = vec2(scale_x, scale_y);\n"
        "    vec4 maskColor = texture(s_TexSampler, scaleCoord);\n"
        "    vec4 originColor = texture(s_TexSampler, v_texCoord);\n"
        // 加权混合
        "    outColor = originColor * (1.0 - alpha) + maskColor * alpha;\n"
        "}\n";


void SoulOutInit(ESContext *esContext) {
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
}

void SoulOutUnInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData) {
        return;
    }
    CameraBaseRelease((UserDataBase *) userData);

    delete userData;
    esContext->userData = nullptr;
}

void SoulOutSetCameraTexId(ESContext *esContext, GLuint texId, int texType, int width, int height,
                           int facing) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData) {
        return;
    }
    int shouldUpdate = CameraBaseUpdateParam((UserDataBase *) userData, texId, texType, width,
                                             height, facing);
    if (shouldUpdate) {
        CameraBaseUpdateMatrix((UserDataBase *) userData, esContext->width, esContext->height);
        SoulOutInit(esContext);
    }
}

void SoulOutDraw(ESContext *esContext) {
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

    //计算offset
    float progress = CameraBaseGetProgress(pUserDataBase, 20, 0, 0, 1);

    glUniform1f(userData->u_offsetLoc, progress);


    CameraBaseAfterDraw(pUserDataBase);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, GL_NONE);
}


}