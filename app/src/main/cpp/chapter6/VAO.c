//
// Created by xucz on 2019-07-25.
//
#include "../common/esUtil.h"

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

int Init(ESContext *esContext) {
    UserData *userData = esContext->userData;
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

    initVAO(esContext);

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
void DrawPrimitiveWithoutVAOs(GLfloat *vertices, GLint vtxStride,
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
DrawPrimitiveWithVAOs(ESContext *esContext, GLint numVertices, GLfloat *vtxBuf, GLint vtxStride,
                      GLint numIndices, GLushort *indices) {
    long start = esGetCurrClockTimeNs();

    UserData *userData = esContext->userData;

    glBindVertexArray(userData->vaoId);

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (const void *) 0);

    glBindVertexArray(0);

    long end = esGetCurrClockTimeNs();

    esLogMessage("DrawPrimitive WithVAOs time:%ld", end - start);
}

void Draw3(ESContext *esContext) {
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

    DrawPrimitiveWithoutVAOs(vertices,
                             sizeof(GLfloat) * (3 + 4),
                             3, indices);

    glUniform1f(userData->offsetX, 1.0f);
    DrawPrimitiveWithVAOs(esContext, 3, vertices,
                          sizeof(GLfloat) * (3 + 4),
                          3, indices);
}

void Shutdown(ESContext *esContext) {
    UserData *userData = esContext->userData;
    glDeleteVertexArrays(1, &userData->vaoId);
    glDeleteBuffers(2, userData->vboIds);
    glDeleteProgram(userData->programObject);
}

int esMain(ESContext *esContext) {
    esContext->userData = malloc(sizeof(UserData));
    esCreateWindow(esContext, "Hello Triangle", 320, 240, ES_WINDOW_RGB);
    if (!Init(esContext)) {
        return GL_FALSE;
    }
    esRegisterShutdownFunc(esContext, Shutdown);
    esRegisterDrawFunc(esContext, Draw3);
    return GL_TRUE;
}
