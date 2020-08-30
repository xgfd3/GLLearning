//
// Created by xucz on 2020/8/30.
//

#include "LUTFilter.h"

extern "C" {
typedef struct __UserData{
    UserDataBase parent;

    GLuint s_TexSamplerLoc;
    GLuint s_LutTextureLoc;

    GLuint lutTexture;

} UserData;

const char f2DShaderStr[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_TexSampler;\n"
        "uniform sampler2D s_LutTexture;\n"
        "\n"
        "vec4 LutFilter(vec2 texCoord){\n"
        "    vec4 textureColor = texture(s_TexSampler, texCoord);\n"// 原始像素RGBA值
        "    float blueColor = textureColor.b * 63.0;\n" //获取 B 分量值，确定 LUT 小方格的 index, 取值范围转为 0～63
        "    vec2 quad1;\n"
        "    quad1.y = floor(floor(blueColor) / 8.0);\n"
        "    quad1.x = floor(blueColor) - (quad1.y * 8.0);\n"
        "    vec2 quad2;\n"
        "    quad2.y = floor(ceil(blueColor) / 7.9999);\n"
        "    quad2.x = ceil(blueColor) - (quad2.y * 8.0);\n"
        "    vec2 texPos1;\n"// 通过 R 和 G 分量的值确定小方格内目标映射的 RGB 组合的坐标，然后归一化，转化为纹理坐标。
        "    texPos1.x = (quad1.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.r);\n"
        "    texPos1.y = (quad1.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.g);\n"
        "    vec2 texPos2;\n"
        "    texPos2.x = (quad2.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.r);\n"
        "    texPos2.y = (quad2.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.g);\n"
        "    vec4 newColor1 = texture(s_LutTexture, texPos1);\n"//取目标映射对应的像素值
        "    vec4 newColor2 = texture(s_LutTexture, texPos2);\n"
        "    vec4 newColor = mix(newColor1, newColor2, fract(blueColor));\n"//使用 Mix 方法对 2 个边界像素值进行混合
        "    return mix(textureColor, vec4(newColor.rgb, textureColor.w), 1.0);"
        "}\n"
        "void main(){ \n"
        "    if(v_texCoord.y > 0.5){\n"
        "        outColor = LutFilter(v_texCoord);\n"
        "    } else {\n"
        "        outColor = texture(s_TexSampler, v_texCoord);\n"
        "    }\n"
        "}";

const char fOESShaderStr[] =
        "#version 300 es\n"
        "#extension GL_OES_EGL_image_external_essl3 : require\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform samplerExternalOES s_TexSampler;\n"
        "uniform sampler2D s_LutTexture;\n"
        "\n"
        "vec4 LutFilter(vec2 texCoord){\n"
        "    vec4 textureColor = texture(s_TexSampler, texCoord);\n"// 原始像素RGBA值
        "    float blueColor = textureColor.b * 63.0;\n" //获取 B 分量值，确定 LUT 小方格的 index, 取值范围转为 0～63
        "    vec2 quad1;\n"
        "    quad1.y = floor(floor(blueColor) / 8.0);\n"
        "    quad1.x = floor(blueColor) - (quad1.y * 8.0);\n"
        "    vec2 quad2;\n"
        "    quad2.y = floor(ceil(blueColor) / 7.9999);\n"
        "    quad2.x = ceil(blueColor) - (quad2.y * 8.0);\n"
        "    vec2 texPos1;\n"// 通过 R 和 G 分量的值确定小方格内目标映射的 RGB 组合的坐标，然后归一化，转化为纹理坐标。
        "    texPos1.x = (quad1.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.r);\n"
        "    texPos1.y = (quad1.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.g);\n"
        "    vec2 texPos2;\n"
        "    texPos2.x = (quad2.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.r);\n"
        "    texPos2.y = (quad2.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.g);\n"
        "    vec4 newColor1 = texture(s_LutTexture, texPos1);\n"//取目标映射对应的像素值
        "    vec4 newColor2 = texture(s_LutTexture, texPos2);\n"
        "    vec4 newColor = mix(newColor1, newColor2, fract(blueColor));\n"//使用 Mix 方法对 2 个边界像素值进行混合
        "    return mix(textureColor, vec4(newColor.rgb, textureColor.w), 1.0);"
        "}\n"
        "void main(){ \n"
        "    if(v_texCoord.y > 0.5){\n"
        "        outColor = LutFilter(v_texCoord);\n"
        "    } else {\n"
        "        outColor = texture(s_TexSampler, v_texCoord);\n"
        "    }\n"
        "}";


void LUTFilterInit(ESContext *esContext){
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
    userData->s_LutTextureLoc = glGetUniformLocation(program, "s_LutTexture");
    
    // 创建一个纹理加载LUT图片
    if(!userData->lutTexture){
        glGenTextures(1, &userData->lutTexture);
        glBindTexture(GL_TEXTURE_2D, userData->lutTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    glBindTexture(GL_TEXTURE_2D, userData->lutTexture);
    auto *imageData = static_cast<NativeImage *>(esContext->imageData[0]);
    if(imageData && imageData->format == IMAGE_FORMAT_RGBA){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageData->width, imageData->height,
                0, GL_RGBA, GL_UNSIGNED_BYTE, imageData->ppPlane[0]);
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void LUTFilterUnInit(ESContext *esContext){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    CameraBaseRelease((UserDataBase *)userData);
    if(userData->lutTexture){
        glDeleteTextures(1, &userData->lutTexture);
        userData->lutTexture = 0;
    }
    delete userData;
    esContext->userData = nullptr;
}

void LUTFilterSetCameraTexId(ESContext *esContext, GLuint texId, int texType, int width, int height, int facing){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    int shouldUpdate = CameraBaseUpdateParam((UserDataBase *) userData, texId, texType, width,
                                             height, facing);
    if(shouldUpdate){
        CameraBaseUpdateMatrix((UserDataBase *)userData, esContext->width, esContext->height);
        LUTFilterInit(esContext);
    }
}

void LUTFilterDraw(ESContext *esContext){
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

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, userData->lutTexture);
    glUniform1i(userData->s_LutTextureLoc, 1);

    CameraBaseAfterDraw(pUserDataBase);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, GL_NONE);
}
}