//
// Created by xucz on 2020/7/25.
//

#include "TextureMap.h"

typedef struct {
    GLuint programObject;

    GLuint textureId;
    GLuint samplerLoc;

} UserData;


void TextureMapInit(ESContext *esContext) {
    UserData *userData = esContext->userData;
    if (userData == NULL) {
        userData = malloc(sizeof(UserData));
        memset(userData, 0, sizeof(UserData));
        esContext->userData = userData;
    }


    // 生成纹理
    glGenTextures(1, &(userData->textureId));
    glBindTexture(GL_TEXTURE_2D, userData->textureId);
    //  设置s轴拉伸为截取
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //  设置t轴拉伸为截取
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //  设置纹理采样方式
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 编译着色器程序
    char vShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec2 a_texCoord;   \n"
            "out vec2 v_texCoord;                       \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = a_position;               \n"
            "   v_texCoord = a_texCoord;                \n"
            "}                                          \n";

    char fShaderStr[] =
            "#version 300 es                           \n"
            "precision mediump float;                           \n"
            "in vec2 v_texCoord;   \n"
            "layout(location = 0) out vec4 outColor;  \n"
            "uniform sampler2D s_TextureMap;  \n"
            "void main()                               \n"
            "{                                         \n"
            "    outColor = texture(s_TextureMap, v_texCoord);               \n"
            "}                                         \n";


    GLuint program = esLoadProgram(vShaderStr, fShaderStr);
    if (program == 0) {
        return;
    }

    userData->programObject = program;
    userData->samplerLoc = glGetUniformLocation(program, "s_TextureMap");

}


void TextureMapDraw(ESContext *esContext) {
    UserData *userData = esContext->userData;
    if (userData == NULL) {
        return;
    }

    if (userData->programObject == GL_NONE || userData->textureId == GL_NONE) {
        return;
    }

    NativeImage* image = esContext->imageData[0];
    if(image == NULL || image->format != IMAGE_FORMAT_RGBA){
        return;
    }

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // 确定纹理坐标和顶点坐标
    GLfloat verticesCoords[] = {
            -1.0f, 0.5f, 0.0f,  // Position 0
            -1.0f, -0.5f, 0.0f,  // Position 1
            1.0f, -0.5f, 0.0f,   // Position 2
            1.0f, 0.5f, 0.0f,   // Position 3
    };

    GLfloat textureCoords[] = {
            0.0f, 0.0f,        // TexCoord 0
            0.0f, 1.0f,        // TexCoord 1
            1.0f, 1.0f,        // TexCoord 2
            1.0f, 0.0f         // TexCoord 3
    };

    GLushort indices[] = {0, 1, 2, 0, 2, 3};

    // 加载图像到纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, image->ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 加载坐标和纹理到着色器程序中
    glUseProgram(userData->programObject);
    // Load the vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT,
                          GL_FALSE, 3 * sizeof(GLfloat), verticesCoords);
    // Load the texture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT,
                          GL_FALSE, 2 * sizeof(GLfloat), textureCoords);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->textureId);
    // Set the RGBA map sampler to texture unit to 0
    glUniform1i(userData->samplerLoc, 0);

    // 绘制
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}

void TextureMapUninit(ESContext *esContext) {
    UserData *userData = esContext->userData;
    if (userData == NULL) {
        return;
    }
    if (userData->textureId) {
        glDeleteTextures(1, &userData->textureId);
    }
    if (userData->programObject) {
        glDeleteProgram(userData->programObject);
    }
    free(userData);
    esContext->userData = NULL;
}


