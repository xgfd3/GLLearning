//
// Created by xucz on 2019-07-25.
//
#include "VBO.h"

typedef struct {
    GLuint programObject;
    GLint offsetX;
    GLuint vboIds[2];
} UserData;

int VBOInit(ESContext *esContext) {
    UserData *userData = esContext->userData;

    if(userData == NULL){
        userData = malloc(sizeof(UserData));
        esContext->userData = userData;
    }

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
 * 绘制单一颜色的三角形
 *
 * @param esContext
 */
void VBODraw1(ESContext *esContext) {
    UserData *userData = esContext->userData;
    GLfloat color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    // 3 vertices, with (x,y,z) per-vertex
    GLfloat vertexPos[3 * 3] =
            {
                    0.0f, 0.5f, 0.0f, // v0
                    -0.5f, -0.5f, 0.0f, // v1
                    0.5f, -0.5f, 0.0f  // v2
            };

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(userData->programObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertexPos);
    glEnableVertexAttribArray(0);

    glVertexAttrib4fv(1, color);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);
}

/**
 * 使用客户顶点数组绘制
 * @param vertices
 * @param vtxStride
 * @param numIndices
 * @param indices
 */
void DrawPrimitiveWithoutVBOs(GLfloat *vertices, GLint vtxStride,
                              GLint numIndices, GLushort *indices) {

    long start = esGetCurrClockTimeNs();

    GLfloat *vtxBuf = vertices;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vtxStride, vtxBuf);

    vtxBuf += 3;

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vtxStride, vtxBuf);

    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indices);

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
DrawPrimitiveWithVBOs(ESContext *esContext, GLint numVertices, GLfloat *vtxBuf, GLint vtxStride,
                      GLint numIndices, GLushort *indices) {
    long start = esGetCurrClockTimeNs();

    UserData *userData = esContext->userData;
    GLuint offset = 0;

    glGenBuffers(2, userData->vboIds);
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, vtxStride * numVertices, vtxBuf, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * numIndices, indices,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vtxStride, (const void *) offset);

    offset += 3 * sizeof(GLfloat);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vtxStride, (const void *) offset);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);


    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    long end = esGetCurrClockTimeNs();

    esLogMessage("DrawPrimitive WithVBOs time:%ld", end - start);
}

void VBODraw2(ESContext *esContext) {
    UserData *userData = esContext->userData;

    GLfloat vertices[3 * (3 + 4)] =
            {
                    -0.5f, 0.5f, 0.0f,
                    1.0f, 0.0f, 0.0f, 1.0f,
                    -1.0f, -0.5f, 0.0f,
                    0.0f, 1.0f, 1.0f, 1.0f,
                    0.0f, -0.5f, 0.0f,
                    0.0f, 0.0f, 1.0f, 1.0f
            };
    GLushort indices[3] = {0, 1, 2};

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(userData->programObject);

    glUniform1f(userData->offsetX, 0);

    DrawPrimitiveWithoutVBOs(vertices,
                             sizeof(GLfloat) * (3 + 4),
                             3, indices);

    glUniform1f(userData->offsetX, 1.0f);
    DrawPrimitiveWithVBOs(esContext, 3, vertices,
                          sizeof(GLfloat) * (3 + 4),
                          3, indices);
}

void VBOShutdown(ESContext *esContext) {
    UserData *userData = esContext->userData;

    glDeleteProgram(userData->programObject);

    free(userData);
    esContext->userData = NULL;
}

