//
// Created by xucz on 2019-07-25.
//
#include "VBO.h"

typedef struct {
    GLuint programObject;
    GLint offsetX;
    GLuint vcVBOId;
    GLuint indicesVBOId;
} UserData;

// 数组数据(cpu)
GLfloat vertices[3 * (3 + 4)] =
        {
                -0.5f, 0.5f, 0.0f,          // v0
                1.0f, 0.0f, 0.0f, 1.0f,     // c0
                -1.0f, -0.5f, 0.0f,         // v1
                0.0f, 1.0f, 1.0f, 1.0f,     // c1
                0.0f, -0.5f, 0.0f,          // v2
                0.0f, 0.0f, 1.0f, 1.0f      // c2
        };
GLushort indices[3] = {0, 1, 2};


int VBOInit(ESContext *esContext) {
    UserData *userData = esContext->userData;

    if(userData == NULL){
        userData = malloc(sizeof(UserData));
        esContext->userData = userData;
    }

    // 使用VBO保存数组数据
    GLuint buffer[2];
    glGenBuffers(2, buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

    userData->vcVBOId = buffer[0];
    userData->indicesVBOId = buffer[1];


    // 编译着色器
    const char vShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec4 a_color;      \n"
            "uniform float offsetX;      \n"
            "out vec4 v_color;                          \n"
            "void main()                                \n"
            "{                                          \n"
            "    v_color = a_color;                     \n"
            "    gl_Position = vec4(a_position.x + offsetX, a_position.y, a_position.z, a_position.w);  \n"
            "}";


    const char fShaderStr[] =
            "#version 300 es            \n"
            "precision mediump float;   \n"
            "in vec4 v_color;           \n"
            "out vec4 o_fragColor;      \n"
            "void main()                \n"
            "{                          \n"
            "    o_fragColor = v_color; \n"
            "}";

    GLuint programObject;

    // Create the program object
    programObject = esLoadProgram(vShaderStr, fShaderStr);

    if (programObject == 0) {
        return GL_FALSE;
    }

    // Store the program object
    userData->programObject = programObject;
    userData->offsetX = glGetUniformLocation(programObject, "offsetX");

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}

/**
 * 使用客户顶点数组绘制
 * @param vertices
 * @param vtxStride
 * @param numIndices
 * @param indices
 */
void DrawPrimitiveWithoutVBOs() {

    long start = esGetCurrClockTimeNs();

    GLfloat *vtxBuf = vertices;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // 顶点坐标
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), vtxBuf);
    // 颜色
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), vtxBuf + 3);

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, indices);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    long end = esGetCurrClockTimeNs();

    esLogMessage("DrawPrimitive WithoutVBOs time:%ld", end - start);
}

/**
 * 使用顶点缓存区对象
 * @param esContext
 * @param numVertices
 * @param vtxBuf
 * @param vtxStride
 * @param numIndices
 * @param indices
 */
void
DrawPrimitiveWithVBOs(GLuint vcVBOId, GLuint indicesVBOId) {
    long start = esGetCurrClockTimeNs();

    GLuint offset = 0;

    glBindBuffer(GL_ARRAY_BUFFER, vcVBOId);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const void *) offset);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat),
            (const void *) (offset + 3 * sizeof(GLfloat)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBOId);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);


    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    long end = esGetCurrClockTimeNs();

    esLogMessage("DrawPrimitive WithVBOs time:%ld", end - start);
}

void VBODraw(ESContext *esContext) {
    UserData *userData = esContext->userData;
    if(userData == NULL){
        return;
    }
    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


    glUseProgram(userData->programObject);

    glUniform1f(userData->offsetX, 0);

    DrawPrimitiveWithoutVBOs();

    glUniform1f(userData->offsetX, 1.0f);
    DrawPrimitiveWithVBOs(userData->vcVBOId, userData->indicesVBOId);
}

void VBOShutdown(ESContext *esContext) {
    UserData *userData = esContext->userData;
    if(userData == NULL){
        return;
    }
    if(userData->programObject){
        glDeleteProgram(userData->programObject);
    }
    if(userData->vcVBOId){
        glDeleteBuffers(1, &userData->vcVBOId);
    }
    if(userData->indicesVBOId){
        glDeleteBuffers(1, &userData->indicesVBOId);
    }

    free(userData);
    esContext->userData = NULL;
}

