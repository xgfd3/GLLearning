//
// Created by xucz on 2020/8/13.
//

#include "Blending.h"

extern "C" {

#define DRAW_TEXTURE_NUM 3

typedef struct __UserData {
    // 程序及程序变量
    GLuint program;
    GLuint u_MVPMatrixLoc;
    GLuint s_TextureMapLoc;

    // VBO坐标
    GLuint vboIds[3];
    GLuint vaoIds[3];

    // 纹理
    GLuint imageTextures[DRAW_TEXTURE_NUM];

    // 窗口偏移坐标
    glm::vec3 windowsTrans[5];

    glm::mat4 mMVPMatrix;
} UserData;


void BlendingInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        userData = new UserData();
        esContext->userData = userData;
    }

    // 编译着色器
    char vShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "layout(location = 0) in vec4 a_position;\n"
            "layout(location = 1) in vec2 a_texCoord;\n"
            "out vec2 v_texCoord;\n"
            "uniform mat4 u_MVPMatrix;\n"
            "void main()\n"
            "{\n"
            "      gl_Position = u_MVPMatrix * a_position;\n"
            "      v_texCoord = a_texCoord;\n"
            "}"
            ;

    char fShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 v_texCoord;\n"
            "layout( location = 0 ) out vec4 outColor;\n"
            "uniform sampler2D s_TextureMap;\n"
            "void main()\n"
            "{\n"
            "     outColor = texture(s_TextureMap, v_texCoord);\n"
            "}"
            ;

    GLuint program = esLoadProgram(vShaderStr, fShaderStr);
    if(!program){
        return;
    }
    userData->program = program;
    userData->u_MVPMatrixLoc = glGetUniformLocation(program, "u_MVPMatrix");
    userData->s_TextureMapLoc = glGetUniformLocation(program, "s_TextureMap");

    // 生成VBO和VAO
    glGenBuffers(3, userData->vboIds);
    glGenVertexArrays(3, userData->vaoIds);
    // 生成三个纹理
    glGenTextures(3, userData->imageTextures);
    for(unsigned int imageTexture : userData->imageTextures){
        glBindTexture(GL_TEXTURE_2D, imageTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    // 1. 立方体
    GLfloat boxVertices[] = {
            //position            //texture coord
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f,    0.0f, 0.0f,
             0.5f, -0.5f, 0.5f,    1.0f, 0.0f,
             0.5f,  0.5f, 0.5f,    1.0f, 1.0f,
             0.5f,  0.5f, 0.5f,    1.0f, 1.0f,
            -0.5f,  0.5f, 0.5f,    0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,    0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,

             0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,   0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,   0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,   1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,   1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f,    0.0f, 1.0f,
             0.5f, 0.5f, -0.5f,    1.0f, 1.0f,
             0.5f, 0.5f,  0.5f,    1.0f, 0.0f,
             0.5f, 0.5f,  0.5f,    1.0f, 0.0f,
            -0.5f, 0.5f,  0.5f,    0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f,    0.0f, 1.0f,
    };

    // 1.1 缓存起数据
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    // 1.2 记录赋值过程
    glBindVertexArray(userData->vaoIds[0]);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 
            5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    // 1.3 立方体纹理
    auto *boxImage = static_cast<NativeImage *>(esContext->imageData[0]);
    glBindTexture(GL_TEXTURE_2D, userData->imageTextures[0]);
    if(boxImage && boxImage->format == IMAGE_FORMAT_RGBA){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     boxImage->width, boxImage->height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, boxImage->ppPlane[0]);
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);


    // 2 地板
    GLfloat flatVertices[] = {
             5.0f, -0.5f,  5.0f,  2.0f,  0.0f,
            -5.0f, -0.5f,  5.0f,  0.0f,  0.0f,
            -5.0f, -0.5f, -5.0f,  0.0f,  2.0f,

             5.0f, -0.5f,  5.0f,  2.0f,  0.0f,
            -5.0f, -0.5f, -5.0f,  0.0f,  2.0f,
             5.0f, -0.5f, -5.0f,  2.0f,  2.0f
    };

    // 2.1 缓存坐标
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(flatVertices), flatVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    // 2.2 记录设置过程
    glBindVertexArray(userData->vaoIds[1]);
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[1]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    // 2.3 加载地板纹理
    auto *flatImage = static_cast<NativeImage *>(esContext->imageData[1]);
    glBindTexture(GL_TEXTURE_2D, userData->imageTextures[1]);
    if(flatImage && flatImage->format == IMAGE_FORMAT_RGBA){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, flatImage->width, flatImage->height,
                0, GL_RGBA, GL_UNSIGNED_BYTE, flatImage->ppPlane[0]);
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 3 窗口
    GLfloat windowVertices[] = {
            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };

    // 3.1 缓存数据
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertices), windowVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    // 3.2 记录设置过程
    glBindVertexArray(userData->vaoIds[2]);
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[2]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    // 3.3 加载窗口纹理
    auto *windowImage = static_cast<NativeImage *>(esContext->imageData[2]);
    glBindTexture(GL_TEXTURE_2D, userData->imageTextures[2]);
    if(windowImage && windowImage->format == IMAGE_FORMAT_RGBA){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowImage->width, windowImage->height,
                0, GL_RGBA, GL_UNSIGNED_BYTE, windowImage->ppPlane[0]);
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 3.4 设置窗口偏移
    userData->windowsTrans[0] = glm::vec3(-0.3f,  0.0f, -2.3f);
    userData->windowsTrans[1] = glm::vec3(-1.5f,  0.0f, -0.48f);
    userData->windowsTrans[2] = glm::vec3( 0.5f,  0.0f, -0.6f);
    userData->windowsTrans[3] = glm::vec3( 1.5f,  0.0f,  0.51f);
    userData->windowsTrans[4] = glm::vec3( 0.0f,  0.0f,  0.7f);


    for (GLuint i = 0; i < 5; i++)
    {
        GLfloat distance = std::sqrt(std::pow(userData->windowsTrans[i].x - 0.5f, 2.0f)
                + std::pow(userData->windowsTrans[i].y - 1.0f, 2.0f) +
                std::pow(userData->windowsTrans[i].z - 3.0f, 2.0f));
        LOGCATE("windowsTrans index: %d, distance: %f", i, distance);
    }
}

void BlendingUnInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }

    if(userData->program){
        glDeleteProgram(userData->program);
        userData->program = 0;
    }
    if(userData->vboIds[0]){
        glDeleteBuffers(3, userData->vboIds);
        userData->vboIds[0] = 0;
    }
    if(userData->vaoIds[0]){
        glDeleteVertexArrays(3, userData->vaoIds);
        userData->vaoIds[0] = 0;
    }
    if(userData->imageTextures[0]){
        glDeleteTextures(3, userData->imageTextures);
        userData->imageTextures[0] = 0;
    }
    delete userData;
    esContext->userData = nullptr;
}

void BlendingUpdateMatrix(ESContext *esContext, int angleX, int angleY, float scale, glm::vec3 trans, float ratio){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }

    angleX = angleX % 360;
    angleY = angleY % 360;

    // 转成弧度
    float radiansX = MATH_PI / 180.f * angleX;
    float radiansY = MATH_PI / 180.f * angleY;

    // 裁剪坐标
    glm::mat4 Projection = glm::perspective(45.f, ratio, 0.1f, 100.f);

    // 观察坐标
    glm::mat4 View = glm::lookAt(
            glm::vec3(0.5, 1, 3),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
            );

    // 世界坐标
    glm::mat4 Model = glm::mat4(1.f);
    Model = glm::scale(Model, glm::vec3(scale, scale, scale));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.f, 0.f, 0.f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.f, 1.f, 0.f));
    Model = glm::translate(Model, trans);

    userData->mMVPMatrix = Projection * View * Model;
}

void BlendingDraw(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData || !userData->program){
        return;
    }

    float ratio = 1.0f * esContext->width / esContext->height;

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(1.f, 1.f, 1.f, 0.f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    // 设置混合因子
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(userData->program);

    // 1 画两个立方体
    glBindVertexArray(userData->vaoIds[0]);

    // 1.1 设置纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->imageTextures[0]);
    glUniform1i(userData->s_TextureMapLoc, 0);

    // 1.2 设置变换矩阵并绘制
    BlendingUpdateMatrix(esContext, 0, 0, 1.0f, glm::vec3(-1.0f, 0.0f, -1.0f), ratio);
    glUniformMatrix4fv(userData->u_MVPMatrixLoc, 1, GL_FALSE, &userData->mMVPMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    GO_CHECK_GL_ERROR();

    BlendingUpdateMatrix(esContext, 0, 0, 1.0f, glm::vec3(2.0f, 0.0f, 0.0f), ratio);
    glUniformMatrix4fv(userData->u_MVPMatrixLoc, 1, GL_FALSE, &userData->mMVPMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(GL_NONE);

    // 2 画地板
    glBindVertexArray(userData->vaoIds[1]);

    // 2.1 设置纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->imageTextures[1]);
    glUniform1i(userData->s_TextureMapLoc, 0);

    // 2.2 设置变换矩阵并绘制
    BlendingUpdateMatrix(esContext, 0, 0, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), ratio);
    glUniformMatrix4fv(userData->u_MVPMatrixLoc, 1, GL_FALSE, &userData->mMVPMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(GL_NONE);

    // 3 绘制窗口
    glBindVertexArray(userData->vaoIds[2]);

    // 3.1 设置纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->imageTextures[2]);
    glUniform1i(userData->s_TextureMapLoc, 0);

    // 3.2 绘制
    int length = sizeof(userData->windowsTrans) / sizeof(glm::vec3);
    for(int i = 0; i < length; i++){
        BlendingUpdateMatrix(esContext, 0, 0 , 1.0, userData->windowsTrans[i], ratio);
        glUniformMatrix4fv(userData->u_MVPMatrixLoc, 1, GL_FALSE, &userData->mMVPMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(GL_NONE);

}

}