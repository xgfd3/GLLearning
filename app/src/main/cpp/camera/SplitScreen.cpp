//
// Created by xucz on 2020/8/30.
//

#include "SplitScreen.h"

extern "C" {
typedef struct __UserData {
    UserDataBase parent;

    GLuint s_TexSamplerLoc;
} UserData;

const char f2DShaderStr[] =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_TexSampler;\n"
        "void main(){ \n"
        "    vec2 newTexCoord = v_texCoord;\n"
        "    if(newTexCoord.x < 0.5){\n"
        "        newTexCoord.x = newTexCoord.x * 2.0;\n"
        "    } else {\n"
        "        newTexCoord.x = (newTexCoord.x - 0.5) * 2.0;\n"
        "    }\n"
        "    if(newTexCoord.y < 0.5){\n"
        "        newTexCoord.y = newTexCoord.y * 2.0;\n"
        "    } else {\n"
        "        newTexCoord.y = (newTexCoord.y - 0.5) * 2.0;\n"
        "    }\n"
        "    outColor = texture(s_TexSampler, newTexCoord);\n"
        "}\n";

const char fOESShaderStr[] =
        "#version 300 es\n"
        "#extension GL_OES_EGL_image_external_essl3 : require\n"
        "precision mediump float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform samplerExternalOES s_TexSampler;\n"
        "void main(){ \n"
        "    vec2 newTexCoord = v_texCoord;\n"
        "    if(newTexCoord.x < 0.5){\n"
        "        newTexCoord.x = newTexCoord.x * 2.0;\n"
        "    } else {\n"
        "        newTexCoord.x = (newTexCoord.x - 0.5) * 2.0;\n"
        "    }\n"
        "    if(newTexCoord.y < 0.5){\n"
        "        newTexCoord.y = newTexCoord.y * 2.0;\n"
        "    } else {\n"
        "        newTexCoord.y = (newTexCoord.y - 0.5) * 2.0;\n"
        "    }\n"
        "    outColor = texture(s_TexSampler, newTexCoord);\n"
        "}\n";

void SplitScreenInit(ESContext *esContext) {
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

void SplitScreenUnInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    CameraBaseRelease((UserDataBase *)userData);

    delete userData;
    esContext->userData = nullptr;
}

void
SplitScreenSetCameraTexId(ESContext *esContext, GLuint texId, int texType, int width, int height,
                          int facing) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    int shouldUpdate = CameraBaseUpdateParam((UserDataBase *) userData, texId, texType, width,
                                             height, facing);
    if(shouldUpdate){
        CameraBaseUpdateMatrix((UserDataBase *)userData, esContext->width, esContext->height);
        SplitScreenInit(esContext);
    }
}

void SplitScreenDraw(ESContext *esContext) {
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