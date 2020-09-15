//
// Created by xucz on 2020/9/7.
//

#include "SeparationShift.h"

extern "C" {

#define OFFSET_STEP 0.0003
    
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
        "void main(){ \n"
        "    vec4 originColor = texture(s_TexSampler, v_texCoord);\n"
        "    vec4 offsetColor0 = texture(s_TexSampler, vec2(v_texCoord.x + u_offset, v_texCoord.y + u_offset));\n" // 向右下方偏移
        "    vec4 offsetColor1 = texture(s_TexSampler, vec2(v_texCoord.x - u_offset, v_texCoord.y - u_offset));\n" // 向左上方偏移
        "    outColor = vec4(originColor.r, offsetColor1.g, offsetColor0.b, originColor.a);\n" // 混合后输出
        "}\n";

const char fOESShaderStr[] =
        "#version 300 es\n"
        "#extension GL_OES_EGL_image_external_essl3 : require\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform samplerExternalOES s_TexSampler;\n"
        "uniform float u_offset;\n"
        "void main(){ \n"
        "    vec4 originColor = texture(s_TexSampler, v_texCoord);\n"
        "    vec4 offsetColor0 = texture(s_TexSampler, vec2(v_texCoord.x + u_offset, v_texCoord.y + u_offset));\n" // 向右下方偏移
        "    vec4 offsetColor1 = texture(s_TexSampler, vec2(v_texCoord.x - u_offset, v_texCoord.y - u_offset));\n" // 向左上方偏移
        "    outColor = vec4(originColor.r, offsetColor1.g, offsetColor0.b, originColor.a);\n" // 混合后输出
        "}\n";


void SeparationShiftInit(ESContext *esContext){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        userData = new UserData();
        esContext->userData = userData;
    }

    // 编译程序
    UserDataBase *pUserDataBase = (UserDataBase *) userData;
    char *fShaderStr;
    if(pUserDataBase->cameraTexType == TEX_TYPE_OES){
        fShaderStr = (char *)fOESShaderStr;
    }else{
        fShaderStr = (char *)f2DShaderStr;
    }
    GLuint program = CameraBaseLoadProgram(pUserDataBase, fShaderStr);
    if(!program){
        return;
    }

    userData->s_TexSamplerLoc = glGetUniformLocation(program, "s_TexSampler");
    userData->u_offsetLoc = glGetUniformLocation(program, "u_offset");
}

void SeparationShiftUnInit(ESContext *esContext){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    CameraBaseRelease((UserDataBase *)userData);

    delete userData;
    esContext->userData = nullptr;
}

void SeparationShiftSetCameraTexId(ESContext *esContext, GLuint texId, int texType,
        int width, int height, int facing){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    int shouldUpdate = CameraBaseUpdateParam((UserDataBase *) userData, texId, texType, width,
                                             height, facing);
    if(shouldUpdate){
        CameraBaseUpdateMatrix((UserDataBase *)userData, esContext->width, esContext->height);
        SeparationShiftInit(esContext);
    }
}

void SeparationShiftDraw(ESContext *esContext){
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

    //计算offset
    float progress = CameraBaseGetProgress(pUserDataBase, 20, 1, 0, 1);

    glUniform1f(userData->u_offsetLoc, 0.01 * progress);


    CameraBaseAfterDraw(pUserDataBase);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, GL_NONE);
}
}