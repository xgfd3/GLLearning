//
// Created by xucz on 2020/8/29.
//

#ifndef ANDROIDOPENGL3DEMO_CAMERABASE_H
#define ANDROIDOPENGL3DEMO_CAMERABASE_H



#ifdef __cplusplus
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>

extern "C" {
#endif

#include "../common/ImageUtils.h"
#include "../common/esUtil.h"

#define PROGRESS_DISTANCE 30000

typedef struct __UserDataBase{
    // 程序
    GLuint program;
    GLuint u_MVPMatrixLoc;

    GLuint vboIds[2];
    GLuint vaoId;

    // Camera Data
    GLuint cameraTexId;
    int cameraTexType;
    int cameraTexWidth;
    int cameraTexHeight;
    int cameraFacing;

#ifdef __cplusplus
    // Camera Matrix
    glm::mat4 m_MVPMatrix;
#endif

    int decrease; // 0:非递减，1: 递减
    long startTime; // progress开始时间 ms
} UserDataBase;

float CameraBaseGetProgress(UserDataBase *userData, int speed, int revert, float min, float max);

GLuint CameraBaseLoadProgram(UserDataBase *userData, char *fShaderStr);

void CameraBaseRelease(UserDataBase *userData);

void CameraBaseBeforeDraw(UserDataBase *userData, int width, int height);

void CameraBaseAfterDraw(UserDataBase *userData);

void CameraBaseUpdateMatrix(UserDataBase *userData,
        int showWidth, int showHeight);

int CameraBaseUpdateParam(UserDataBase *userData,
        int texId, int texType, int width, int height, int facing);

#ifdef __cplusplus
}
#endif

#endif //ANDROIDOPENGL3DEMO_CAMERABASE_H
