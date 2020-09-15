//
// Created by xucz on 2020/9/15.
//

#include "PictInPict.h"

extern "C" {
typedef struct __UserData {
    UserDataBase parent;

    GLuint s_TexSamplerLoc;
} UserData;

const char f2DShaderStr[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_TexSampler;\n"
        "\n"
        "vec2 scale(vec2 uv, float level){\n"
        "    vec2 center = vec2(0.5, 0.5);\n"
        "    vec2 newTexCoord = uv.xy;\n"
        "    newTexCoord -= center;\n"
        "    newTexCoord = newTexCoord / level;\n"
        "    newTexCoord += center;\n"
        "    return newTexCoord;\n"
        "}\n"
        "\n"
        "const float OFFSET_LEVEL = 0.15;\n"
        "const float SCALE_LEVEL = 4.0;\n"
        "void main(){ \n"
        "    if(OFFSET_LEVEL < v_texCoord.x && v_texCoord.x < (1.0 - OFFSET_LEVEL) \n"
        "         && OFFSET_LEVEL < v_texCoord.y && v_texCoord.y < (1.0 - OFFSET_LEVEL)){\n"
        "         vec2 newTexCoord = v_texCoord;\n"
        "         newTexCoord -= OFFSET_LEVEL;\n"
        "         newTexCoord = newTexCoord / (1.0 - 2.0*OFFSET_LEVEL);\n"
        "         outColor = texture(s_TexSampler, newTexCoord);\n"
        "    } else {\n"
        "         outColor = texture(s_TexSampler, scale(v_texCoord, SCALE_LEVEL));\n"
        "    }\n"
        "}\n";

const char fOESShaderStr[] =
        "#version 300 es\n"
        "#extension GL_OES_EGL_image_external_essl3 : require\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform samplerExternalOES s_TexSampler;\n"
        "\n"
        "vec2 scale(vec2 uv, float level){\n"
        "    vec2 center = vec2(0.5, 0.5);\n"
        "    vec2 newTexCoord = uv.xy;\n"
        "    newTexCoord -= center;\n"
        "    newTexCoord = newTexCoord / level;\n"
        "    newTexCoord += center;\n"
        "    return newTexCoord;\n"
        "}\n"
        "\n"
        "const float OFFSET_LEVEL = 0.15;\n"
        "const float SCALE_LEVEL = 4.0;\n"
        "void main(){ \n"
        "    if(OFFSET_LEVEL < v_texCoord.x && v_texCoord.x < (1.0 - OFFSET_LEVEL) \n"
        "         && OFFSET_LEVEL < v_texCoord.y && v_texCoord.y < (1.0 - OFFSET_LEVEL)){\n"
        "         vec2 newTexCoord = v_texCoord;\n"
        "         newTexCoord -= OFFSET_LEVEL;\n"
        "         newTexCoord = newTexCoord / (1.0 - 2.0*OFFSET_LEVEL);\n"
        "         outColor = texture(s_TexSampler, newTexCoord);\n"
        "    } else {\n"
        "         outColor = texture(s_TexSampler, scale(v_texCoord, SCALE_LEVEL));\n"
        "    }\n"
        "}\n";

void PictInPictInit(ESContext *esContext) {
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
}

void PictInPictUnInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    CameraBaseRelease((UserDataBase *)userData);

    delete userData;
    esContext->userData = nullptr;
}

void PictInPictSetCameraTexId(ESContext *esContext,
                              GLuint texId, int texType,
                              int width, int height, int facing) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    int shouldUpdate = CameraBaseUpdateParam((UserDataBase *) userData, texId, texType, width,
                                             height, facing);
    if(shouldUpdate){
        CameraBaseUpdateMatrix((UserDataBase *)userData, esContext->width, esContext->height);
        PictInPictInit(esContext);
    }
}

void PictInPictDraw(ESContext *esContext) {
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

    CameraBaseAfterDraw(pUserDataBase);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, GL_NONE);
}

}