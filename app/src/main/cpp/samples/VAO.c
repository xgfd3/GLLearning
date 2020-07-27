//
// Created by xucz on 2019-07-25.
//
#include "VAO.h"

typedef struct {
    GLuint programObject;
    GLint offsetX;
    GLuint vboIds[2];
    GLuint vaoId;
} UserData;


void initVAO(ESContext *esContext) {
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

    glGenBuffers(2, userData->vboIds);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &userData->vaoId);

    glBindVertexArray(userData->vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, (const void *) 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7,
                          (const void *) (3 * sizeof(GLfloat)));

    glBindVertexArray(0);
}

void VAOInitialize(ESContext *esContext) {
    UserData *userData = esContext->userData;
    if(userData == NULL){
        userData = malloc(sizeof(UserData));
        esContext->userData = userData;
    }

    // 生成VBO以及VAO
    initVAO(esContext);

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
        return;
    }

    // Store the program object
    userData->programObject = programObject;
    userData->offsetX = glGetUniformLocation(programObject, "offsetX");

}

/**
 * 使用VBO绘制
 *
 * @param vertices
 * @param vtxStride
 * @param numIndices
 * @param indices
 */
void DrawPrimitiveWithoutVAOs(GLuint vcVBOId, GLuint indicesVBOId) {

    long start = esGetCurrClockTimeNs();


    glBindBuffer(GL_ARRAY_BUFFER, vcVBOId);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7,
                          (const void *) (sizeof(GLfloat) * 3));


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBOId);

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    long end = esGetCurrClockTimeNs();

    esLogMessage("DrawPrimitive WithoutVAOs time:%ld", end - start);
}

/**
 * 使用VAO绘制
 *
 * @param esContext
 * @param numVertices
 * @param vtxBuf
 * @param vtxStride
 * @param numIndices
 * @param indices
 */
void DrawPrimitiveWithVAOs(GLuint vaoId) {
    long start = esGetCurrClockTimeNs();

    glBindVertexArray(vaoId);

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (const void *) 0);

    glBindVertexArray(0);

    long end = esGetCurrClockTimeNs();

    esLogMessage("DrawPrimitive WithVAOs time:%ld", end - start);
}

void VAODraw(ESContext *esContext) {
    UserData *userData = esContext->userData;
    if(userData == NULL){
        return;
    }

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(userData->programObject);

    glUniform1f(userData->offsetX, 0);

    DrawPrimitiveWithoutVAOs(userData->vboIds[0], userData->vboIds[1]);

    glUniform1f(userData->offsetX, 1.0f);
    DrawPrimitiveWithVAOs(userData->vaoId);
}

void VAOuninit(ESContext *esContext) {
    UserData *userData = esContext->userData;
    if(userData == NULL){
        return;
    }
    if(userData->vaoId){
        glDeleteVertexArrays(1, &userData->vaoId);
    }
    if(userData->vboIds[0] && userData->vboIds[1]){
        glDeleteBuffers(2, userData->vboIds);
    }
    if(userData->programObject){
        glDeleteProgram(userData->programObject);
    }
    free(userData);
    esContext->userData = NULL;
}

