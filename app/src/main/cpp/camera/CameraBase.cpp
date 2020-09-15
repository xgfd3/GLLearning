//
// Created by xucz on 2020/8/29.
//

#include "CameraBase.h"

extern "C" {

GLuint CameraBaseLoadProgram(UserDataBase *userData, char *fShaderStr) {
    if (!userData) {
        return 0;
    }

    if (userData->program) {
        CameraBaseRelease(userData);
    }

    // 编译程序
    char vShaderStr[] =
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


    GLuint program = esLoadProgram(vShaderStr, fShaderStr);
    if (!program) {
        return 0;
    }

    userData->program = program;
    userData->u_MVPMatrixLoc = glGetUniformLocation(program, "u_MVPMatrix");

    // 确定顶点坐标和纹理坐标
    GLfloat _screenvertices[] = {
            -1.0f, 1.0f, 0, // v1
            0, 0,  // t1
            -1.0f, -1.0f, 0, // v2
            0, 1.0f, // t2
            1.0f, -1.0f, 0, // v3
            1.0f, 1.0f, // t3
            1.0f, 1.0f, 0, //v4
            1.0f, 0 // t4
    };
    GLushort screenindices[] = {0, 1, 2, 0, 2, 3};

    // 使用
    // 使用VBO缓存数组数据
    glGenBuffers(2, userData->vboIds);
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_screenvertices), _screenvertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(screenindices), screenindices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

    // 使用VAO记录赋值过程
    glGenVertexArrays(1, &userData->vaoId);
    glBindVertexArray(userData->vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          ((void *) (3 * sizeof(GLfloat))));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);

    glBindVertexArray(GL_NONE);

    return program;
}


void CameraBaseBeforeDraw(UserDataBase *userData, int width, int height) {
    if (!userData || !userData->program) {
        return;
    }
    if (!userData->cameraTexId) {
        return;
    }

    // 清空画布
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.f, 1.f, 1.f, 0.f);


    glUseProgram(userData->program);
    glBindVertexArray(userData->vaoId);

    glUniformMatrix4fv(userData->u_MVPMatrixLoc, 1, GL_FALSE, &userData->m_MVPMatrix[0][0]);
}

void CameraBaseAfterDraw(UserDataBase *userData) {
    if (!userData || !userData->program) {
        return;
    }
    if (!userData->cameraTexId) {
        return;
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    //GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);
}

void CameraBaseRelease(UserDataBase *userData) {
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
}

void CameraBaseUpdateMatrix(UserDataBase *userData,
                            int showWidth, int showHeight) {
    glm::mat4 Model = glm::mat4(1.f);
    if (userData->cameraFacing == CAMERA_FACING_BACK) {
        // 后置摄像头
        float angle = -1.f * MATH_PI / 180.f * 90;
        Model = glm::rotate(Model, angle, glm::vec3(0, 0, 1.f));
        float texRatio = 1.0f * userData->cameraTexHeight / userData->cameraTexWidth;
        float showRatio = 1.0f * showWidth / showHeight;

        float scaleY = texRatio / showRatio;
        LOGCATE("OES2Screen -- scaleY:%f", scaleY);
        Model = glm::scale(Model, glm::vec3(1.0f, 1.0f, scaleY));
    } else if (userData->cameraFacing == CAMERA_FACING_FRONT) {
        // 前置摄像头
        float angle = MATH_PI / 180.f * 90;
        Model = glm::rotate(Model, angle, glm::vec3(0, 0, 1.f));
        float texRatio = 1.0f * userData->cameraTexHeight / userData->cameraTexWidth;
        float showRatio = 1.0f * showWidth / showHeight;

        float scaleY = texRatio / showRatio;
        LOGCATE("OES2Screen -- scaleY:%f", scaleY);
        Model = glm::scale(Model, glm::vec3(1.0f, 1.0f, scaleY));
        // 左右镜像
        Model = glm::scale(Model, glm::vec3(1.0f, -1.0f, 1.0f));
    }
    userData->m_MVPMatrix = Model;
}

int CameraBaseUpdateParam(UserDataBase *userData, int texId, int texType, int width, int height,
                           int facing) {
    if (!userData) {
        return 0;
    }
    int shouldUpdate = 0;

    if (userData->cameraTexId != texId
        || userData->cameraTexType != texType
        || userData->cameraTexWidth != width
        || userData->cameraTexHeight != height
        || userData->cameraFacing != facing) {
        shouldUpdate = 1;
    }

    userData->cameraTexId = texId;
    userData->cameraTexType = texType;
    userData->cameraTexWidth = width;
    userData->cameraTexHeight = height;
    userData->cameraFacing = facing;

    return shouldUpdate;
}

float CameraBaseGetProgress(UserDataBase *userData, int speed, int revert, float min, float max){
    if(!userData){
        return 0;
    }
    
    // 计算从0开始的时长ms
    long currTimeMs = esGetCurrClockTimeNs() / 1000000;
    if(userData->startTime == 0 ){
        userData->startTime = currTimeMs;
    }
    long diffTimeMs = currTimeMs - userData->startTime;

    long diffDs = diffTimeMs * speed;

    float progress = 1.0f * diffDs / PROGRESS_DISTANCE;
    if(userData->decrease){
        progress = max - progress;
    }

    if(progress > max){
        userData->startTime = currTimeMs;
        progress = max;
        if(revert){
            userData->decrease = 1;
        }
    }
    if(progress < min){
        userData->startTime = currTimeMs;
        progress = min;
        if(revert){
            userData->decrease = 0;
        }
    }

    //LOGCATE("CameraBaseGetProgress diffTimeMs:%ld, diffDs:%ld, progress:%.2f", diffTimeMs, diffDs, progress);
    return progress;
}
}