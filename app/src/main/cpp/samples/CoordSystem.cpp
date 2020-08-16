//
// Created by xucz on 2020/8/8.
//

#include "CoordSystem.h"

extern "C" {

typedef struct _UserData {
    GLuint imageTexture;

    GLuint program;
    GLuint mvpMatrixLoc;
    GLuint textureLoc;

    GLuint vboIds[2];
    GLuint vaoId;

    glm::mat4 m_MVPMatrix;
    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

} UserData;


void CoorderSystemInit(ESContext *context) {
    auto *userData = static_cast<UserData *>(context->userData);
    if (!userData) {
        userData = new UserData();
        context->userData = userData;
    }

    userData->m_AngleX = 0;
    userData->m_AngleY = 0;
    userData->m_ScaleY = 1.0f;
    userData->m_ScaleX = 1.0f;

    // 创建一个纹理，将将图片加载到纹理上
    glGenTextures(1, &userData->imageTexture);
    glBindTexture(GL_TEXTURE_2D, userData->imageTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto *imageData = static_cast<NativeImage *>(context->imageData[0]);
    if (imageData && imageData->format == IMAGE_FORMAT_RGBA) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageData->width, imageData->height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, imageData->ppPlane[0]);
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);


    // 编译着色器
    const char vShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec2 a_texCoord;   \n"
            "uniform mat4 u_MVPMatrix;                  \n"
            "out vec2 v_texCoord;                       \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = u_MVPMatrix * a_position; \n"
            "   v_texCoord = a_texCoord;                \n"
            "}                                          \n";

    char fShaderStr[] =
            "#version 300 es                                  \n"
            "precision mediump float;                         \n"
            "in vec2 v_texCoord;                              \n"
            "layout(location = 0) out vec4 outColor;          \n"
            "uniform sampler2D s_TextureMap;                  \n"
            "void main()                                      \n"
            "{                                                \n"
            "    outColor = texture(s_TextureMap, v_texCoord);\n"
            "}                                                \n";
    GLuint program = esLoadProgram(vShaderStr, fShaderStr);
    if(!program){
        return;
    }
    userData->program = program;
    userData->mvpMatrixLoc = glGetUniformLocation(program, "u_MVPMatrix");
    userData->textureLoc = glGetUniformLocation(program, "s_TextureMap");

    // 确定顶点和纹理坐标
    GLfloat coords[] = {
            -1.0f, 1.0f, 0.0f,
            0.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 0.0f
    };
    GLushort indices[] ={0, 1, 2, 0, 2, 3};

    // 使用VBO保存坐标数组
    glGenBuffers(2, userData->vboIds);
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

    // 使用VAO记录坐标设置过程
    glGenVertexArrays(1, &userData->vaoId);
    glBindVertexArray(userData->vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(0 + 3* sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
    glBindVertexArray(GL_NONE);

}

void CoorderSystemUnInit(ESContext *context) {
    auto *userData = static_cast<UserData *>(context->userData);
    if (!userData) {
        return;
    }
    if(userData->vaoId){
        glDeleteVertexArrays(1, &userData->vaoId);
        userData->vaoId = 0;
    }
    if( userData->vboIds[0] ){
        glDeleteBuffers(2, userData->vboIds);
        userData->vboIds[0] = 0;
    }
    if(userData->program){
        glDeleteProgram(userData->program);
        userData->program = 0;
    }
    if(userData->imageTexture){
        glDeleteTextures(1, &userData->imageTexture);
        userData->imageTexture = 0;
    }
    delete userData;
    context->userData = nullptr;
}

void CoordSystemUpdateMvpMatrix(ESContext *context){
    auto *userData = static_cast<UserData *>(context->userData);
    if(!userData){
        return;
    }

    int angleX = userData->m_AngleX;
    int angleY = userData->m_AngleY;
    float scaleX = userData->m_ScaleX;
    float scaleY = userData->m_ScaleY;
    float ratio = context->width * 1.0f / context->height;

    LOGCATE("CoordSystemSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f, scaleX = %f, scaleY = %f",
            angleX, angleY, ratio, scaleX, scaleY);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(M_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(M_PI / 180.0f * angleY);


    // Projection matrix 裁剪坐标
    //   正交投影
    //glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.5f, 1.5f, 0.1f, 100.0f);
    //   视锥投影
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.5f, 1.5f, 4.0f, 100.0f);
    //   透视投影
    glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix 观察坐标
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix 世界坐标
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scaleX, scaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    userData->m_MVPMatrix = Projection * View * Model;
}

void CoorderSystemDraw(ESContext *context) {
    auto *userData = static_cast<UserData *>(context->userData);
    if (!userData) {
        return;
    }

    glViewport(0, 0, context->width, context->height);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(userData->program);

    // 设置MVP和textureId
    CoordSystemUpdateMvpMatrix(context);

    glBindVertexArray(userData->vaoId);

    glUniformMatrix4fv(userData->mvpMatrixLoc, 1, GL_FALSE,
            &userData->m_MVPMatrix[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->imageTexture);
    glUniform1i(userData->textureLoc, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
    GO_CHECK_GL_ERROR();

}

void CoorderSystemUpdateTransformMatrix(ESContext *context,
                                        GLfloat rotateX, GLfloat rotateY,
                                        GLfloat scaleX, GLfloat scaleY) {
    auto *userData = static_cast<UserData *>(context->userData);
    if(!userData){
        return;
    }
    userData->m_AngleX = rotateX;
    userData->m_AngleY = rotateY;
    userData->m_ScaleX = scaleX;
    userData->m_ScaleY = scaleY;
    CoordSystemUpdateMvpMatrix(context);
}

}