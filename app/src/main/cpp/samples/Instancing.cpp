//
// Created by xucz on 2020/8/13.
//

#include "Instancing.h"

extern "C" {

typedef struct __UserData {


    GLuint program;

    GLuint vboIds[2];
    GLuint vaoId;


} UserData;

void InstancingInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData) {
        userData = new UserData();
        esContext->userData = userData;
    }

    // 编译着色器
    char vShaderStr[] =
            "#version 300 es\n"
            "layout(location = 0) in vec2 vPosition;\n"
            "layout(location = 1) in vec3 color;\n"
            "layout(location = 2) in vec2 offset;\n"
            "out vec3 fColor;\n"
            "void main()\n"
            "{\n"
            "    float instanceID = float(gl_InstanceID);\n"
            "    vec2 pos = vPosition * (instanceID / 100.0);\n"
            "    gl_Position = vec4(pos + offset, 0.0, 1.0);\n"
            "    fColor = color;\n"
            "}";

    char fShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "out vec4 fragColor;\n"
            "in  vec3 fColor;\n"
            "void main()\n"
            "{\n"
            "    fragColor = vec4(fColor, 1.0);\n"
            "}";

    GLuint program = esLoadProgram(vShaderStr, fShaderStr);
    if (!program) {
        return;
    }

    userData->program = program;

    // 确定顶点坐标、色值和偏移坐标
    GLfloat vVertices[] = {
            // Positions     // Colors
            -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
             0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
            -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

            -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
             0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
             0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };
    glm::vec2 translations[100];
    int index = 0;
    GLfloat offset = 0.1f;
    for (GLint y = -10; y < 10; y += 2) {
        for (GLint x = -10; x < 10; x += 2) {
            glm::vec2 translation;
            translation.x = (GLfloat) x / 10.0f + offset;
            translation.y = (GLfloat) y / 10.0f + offset;
            translations[index++] = translation;
        }
    }

    // 使用VBO缓存数据
    glGenBuffers(2, userData->vboIds);
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, translations, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    // 使用VAO记录赋值过程
    glGenVertexArrays(1, &userData->vaoId);

    glBindVertexArray(userData->vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (void *) (2 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[1]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glVertexAttribDivisor(2, 1);
    glBindVertexArray(GL_NONE);
}

void InstancingUnInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData) {
        return;
    }

    if (userData->program) {
        glDeleteProgram(userData->program);
        userData->program = 0;
    }
    if (userData->vboIds[0]) {
        glDeleteBuffers(2, userData->vboIds);
        userData->vboIds[0] = 0;
    }
    if (userData->vaoId) {
        glDeleteVertexArrays(1, &userData->vaoId);
        userData->vaoId = 0;
    }
    delete userData;
    esContext->userData = nullptr;
}

void InstancingDraw(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData || !userData->program) {
        return;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.03f, 0.03f, 0.03f, 1.0f);

    glUseProgram(userData->program);

    glBindVertexArray(userData->vaoId);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
    glBindVertexArray(GL_NONE);

}

void InstancingUpdateTransformMatrix(ESContext *esContext, GLfloat angleX, GLfloat angleY,
                                     GLfloat scaleX, GLfloat scaleY) {

}

}