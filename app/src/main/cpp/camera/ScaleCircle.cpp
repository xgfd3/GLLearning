//
// Created by xucz on 2020/8/29.
//

#include "ScaleCircle.h"

extern "C" {

typedef struct __UserData {
    UserDataBase parent;

    GLuint s_TexSamplerLoc;
    GLuint u_offsetLoc;
    GLuint texSizeLoc;

    glm::vec2 texSize;

    float progress;
    float proStep;

} UserData;

const char f2DShaderStr1[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_TexSampler;\n"
        "uniform vec2 texSize;\n"
        "uniform float u_offset;\n"
        "void main(){ \n"
        "    vec2 imgTexCoord = v_texCoord * texSize;\n"
        "    float r = (u_offset + 0.208) * texSize.x / 2.0;\n"
        "    if(distance(imgTexCoord, vec2(texSize.x / 2.0, texSize.y / 2.0)) < r){\n"
        "        outColor = texture(s_TexSampler, v_texCoord);\n"
        "    } else {\n"
        "        outColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "    }\n"
        "}\n";

const char fOESShaderStr1[] =
        "#version 300 es\n"
        "#extension GL_OES_EGL_image_external_essl3 : require\n"
        "precision highp float;\n" // !! 注意精度，如果精度不能满足计算需要，会出现计算错误，使用distance时要使用到highp精度
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform samplerExternalOES s_TexSampler;\n"
        "uniform vec2 texSize;\n"
        "uniform float u_offset;\n"
        "void main(){ \n"
        "    vec2 imgTex = v_texCoord * texSize;\n"
        "    float r = (u_offset + 0.208) * texSize.x / 2.0;\n"
        "    if(distance(imgTex, vec2(texSize.x / 2.0, texSize.y / 2.0)) < r){\n"
        "        outColor = texture(s_TexSampler, v_texCoord);\n"
        "    } else {\n"
        "        outColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "    }\n"
        "}\n";


void ScaleCircleInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        userData = new UserData();
        esContext->userData = userData;
    }

    // 编译程序
    UserDataBase *pUserDataBase = (UserDataBase *) userData;
    char *fShaderStr;
    if(pUserDataBase->cameraTexType == TEX_TYPE_OES){
        fShaderStr = (char *)fOESShaderStr1;
    }else{
        fShaderStr = (char *)f2DShaderStr1;
    }
    
    GLuint program = CameraBaseLoadProgram(pUserDataBase, fShaderStr);
    if(!program){
        return;
    }
    userData->s_TexSamplerLoc = glGetUniformLocation(program, "s_TexSampler");
    userData->u_offsetLoc = glGetUniformLocation(program, "u_offset");
    userData->texSizeLoc = glGetUniformLocation(program, "texSize");
}

void ScaleCircleUnInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    CameraBaseRelease((UserDataBase *)userData);

    delete userData;
    esContext->userData = nullptr;
}

void ScaleCircleSetCameraTexId(ESContext *esContext,
                          GLuint texId, int texType, int width, int height,
                          int facing) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    int shouldUpdate = CameraBaseUpdateParam((UserDataBase *) userData, texId, texType, width,
                                              height, facing);
    if(shouldUpdate){
        CameraBaseUpdateMatrix((UserDataBase *)userData, esContext->width, esContext->height);
        userData->texSize = glm::vec2(1.0f * width, 1.0f * height);
        ScaleCircleInit(esContext);
    }
}

void ScaleCircleDraw(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }

    UserDataBase *pUserDataBase = (UserDataBase *) userData;
    CameraBaseBeforeDraw(pUserDataBase, esContext->width, esContext->height);
    if(!pUserDataBase->cameraTexId){
        return;
    }

    if(pUserDataBase->cameraTexType == TEX_TYPE_OES){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, pUserDataBase->cameraTexId);
        glUniform1i(userData->s_TexSamplerLoc, 0);
    }
    else{
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pUserDataBase->cameraTexId);
        glUniform1i(userData->s_TexSamplerLoc, 0);
    }

    // 图片坐标
    glUniform2fv(userData->texSizeLoc, 1, &userData->texSize[0]);

    // 偏移
    float progress = userData->progress;
    if (userData->proStep == 0.f) {
        userData->proStep = 0.02f;
    }

    float offset = progress;
    glUniform1f(userData->u_offsetLoc, offset);

    userData->progress = progress + userData->proStep;
    if (userData->progress >= 1) {
        userData->proStep = -0.02f;
    } else if (userData->progress <= 0) {
        userData->proStep = 0.02f;
    }

    CameraBaseAfterDraw(pUserDataBase);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, GL_NONE);
}


}