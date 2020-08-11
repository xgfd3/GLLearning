//
// Created by xucz on 2020/7/26.
//

#include "YUVTextureMap.h"

typedef struct __UserData{

    GLuint program;

    GLuint yTextureId;
    GLuint uvTextureId;
    
    GLint yTextureLocation;
    GLint uvTextureLocation;

} UserData;

void YUVTextureMapInit(ESContext* esContext){
    UserData *userData = esContext->userData;
    if(userData == NULL){
        userData = malloc(sizeof(UserData));
        memset(userData, 0, sizeof(UserData));
        esContext->userData = userData;
    }

    // 生成两个纹理，一个保存Y分量，一个保存UV分量
    GLuint textures[2];
    glGenTextures(2, textures);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    userData->yTextureId = textures[0];
    userData->uvTextureId = textures[1];

    // 编译着色器
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
            "#version 300 es                                     \n"
            "precision mediump float;                            \n"
            "in vec2 v_texCoord;                                 \n"
            "layout(location = 0) out vec4 outColor;             \n"
            "uniform sampler2D y_texture;                        \n"
            "uniform sampler2D uv_texture;                        \n"
            "void main()                                         \n"
            "{                                                   \n"
            "   vec3 yuv;                                       \n"
            "   yuv.x = texture(y_texture, v_texCoord).r;   \n"
            "   yuv.y = texture(uv_texture, v_texCoord).a-0.5;  \n"
            "   yuv.z = texture(uv_texture, v_texCoord).r-0.5;  \n"
            "   highp vec3 rgb = mat3( 1,       1,          1,                  \n"
            "               0,      -0.344,     1.770,                  \n"
            "               1.403,  -0.714,       0) * yuv;             \n"
            "   outColor = vec4(rgb, 1);                        \n"
            "}                                                   \n";
    
    GLuint program = esLoadProgram(vShaderStr, fShaderStr);
    if(program == 0){
        return;
    }
    
    userData->program = program;
    userData->yTextureLocation = glGetUniformLocation(program, "y_texture");
    userData->uvTextureLocation = glGetUniformLocation(program, "uv_texture");

}

void YUVTextureMapDraw(ESContext* esContext){
    UserData *userData = esContext->userData;
    if(userData == NULL){
        return;
    }

    NativeImage *image = esContext->imageData;
    if(image == NULL){
        return;
    }

    if(image->format != IMAGE_FORMAT_NV21){
        return;
    }

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // 确定纹理坐标和顶点坐标
    GLfloat verticesCoords[] = {
            -1.0f, 0.78f, 0.0f,  // Position 0
            -1.0f, -0.78f, 0.0f,  // Position 1
            1.0f, -0.78f, 0.0f,   // Position 2
            1.0f, 0.78f, 0.0f,   // Position 3
    };

    GLfloat textureCoords[] = {
            0.0f, 0.0f,        // TexCoord 0
            0.0f, 1.0f,        // TexCoord 1
            1.0f, 1.0f,        // TexCoord 2
            1.0f, 0.0f         // TexCoord 3
    };

    GLushort indices[] = {0, 1, 2, 0, 2, 3};

    // 加载图像数据到纹理中
    glBindTexture(GL_TEXTURE_2D, userData->yTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, image->width, image->height,
            0, GL_LUMINANCE, GL_UNSIGNED_BYTE, image->ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    glBindTexture(GL_TEXTURE_2D, userData->uvTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, image->width >> 1, image->height >> 1,
                 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, image->ppPlane[1]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    
    // 将纹理和坐标设置给着色器
    glUseProgram(userData->program);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), verticesCoords);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), textureCoords);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->yTextureId);
    glUniform1i(userData->yTextureLocation, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, userData->uvTextureId);
    glUniform1i(userData->uvTextureLocation, 1);
    
    // 绘制
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

}

void YUVTextureMapUnInit(ESContext* esContext){
    UserData *userData = esContext->userData;
    if(userData == NULL){
        return;
    }
    if(userData->program){
        glDeleteProgram(userData->program);
    }
    if(userData->yTextureId){
        glDeleteTextures(1, &userData->yTextureId);
    }
    if(userData->uvTextureId){
        glDeleteTextures(1, &userData->uvTextureId);
    }
    free(userData);
    esContext->userData = NULL;
}