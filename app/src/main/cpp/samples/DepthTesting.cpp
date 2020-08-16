//
// Created by xucz on 2020/8/9.
//

#include "DepthTesting.h"

extern "C" {

glm::vec3 transPos[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  2.0f, -1.0f) * 1.6f,
        glm::vec3(-1.5f, -2.2f, -1.5f) * 1.6f,
        glm::vec3(-1.8f, -2.0f,  1.3f) * 1.6f,
        glm::vec3( 1.4f, -1.4f, -1.5f) * 1.6f,
        glm::vec3(-1.7f,  2.0f, -1.5f) * 1.6f,
        glm::vec3( 1.3f, -2.0f,  2.5f) * 1.6f,
        glm::vec3( 0.5f,  1.3f, -0.1f) * 1.6f,
        glm::vec3( 1.5f,  2.2f,  1.5f) * 1.6f,
        glm::vec3(-1.3f,  1.0f, -1.5f) * 1.6f,
        glm::vec3(-1.3f,  0.0f, -1.5f) * 1.6f,
        glm::vec3( 0.0f, -1.3f, -0.5f) * 1.6f,
        glm::vec3( 0.0f, -1.5f,  1.5f) * 1.6f,
};

typedef struct __UserData{
    GLuint imageTexture;


    GLuint program;
    GLuint mMVPMatrixLoc;
    GLuint textureLoc;

    GLuint vboId;
    GLuint vaoId;

    glm::mat4 mMVPMatrix;
    int angleX;
    int angleY;
} UserData;

void DepthTestingInit(ESContext *esContext){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        userData = new UserData();
        esContext->userData = userData;
    }

    userData->mMVPMatrix = glm::mat4(0.0f);

    // 生成一个纹理并加载图片
    glGenTextures(1, &userData->imageTexture);
    glBindTexture(GL_TEXTURE_2D, userData->imageTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    auto *image = static_cast<NativeImage *>(esContext->imageData[0]);
    if(image && image->format == IMAGE_FORMAT_RGBA){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 
                0, GL_RGBA, GL_UNSIGNED_BYTE, image->ppPlane[0]);
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    
    // 编译着色器
    char vShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "layout(location = 0) in vec4 a_position;\n"
            "layout(location = 1) in vec2 a_texCoord;\n"
            "\n"
            "out vec2 v_texCoord;\n"
            "uniform mat4 u_MVPMatrix;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_MVPMatrix * a_position;\n"
            "    v_texCoord = a_texCoord;\n"
            "}";

    char fShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 v_texCoord;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform sampler2D s_TextureMap;\n"
            "void main()\n"
            "{\n"
            "    vec4 objectColor = vec4(vec3(gl_FragCoord.z), 1.0f);\n"
            "    outColor = objectColor;\n"
            "}";
    GLuint program = esLoadProgram(vShaderStr, fShaderStr);
    if(!program){
        return;
    }
    userData->program = program;
    userData->mMVPMatrixLoc = glGetUniformLocation(program, "u_MVPMatrix");
    userData->textureLoc = glGetUniformLocation(program, "s_TextureMap");

    // 确定纹理坐标和顶点坐标
    GLfloat vertices[] = {
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

    // 使用VBO保存数组数据
    glGenBuffers(1, &userData->vboId);
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    // 使用VAO记录数组设置过程
    glGenVertexArrays(1, &userData->vaoId);
    glBindVertexArray(userData->vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboId);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
            (void *)(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);
    
}

void DepthTestingUnInit(ESContext *esContext){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    if(userData->imageTexture){
        glDeleteTextures(1, &userData->imageTexture);
        userData->imageTexture = 0;
    }
    if(userData->vboId){
        glDeleteBuffers(1, &userData->vboId);
        userData->vboId = 0;
    }
    if(userData->vaoId){
        glDeleteVertexArrays(1, &userData->vaoId);
        userData->vaoId = 0;
    }
    if(userData->program){
        glDeleteProgram(userData->program);
        userData->program = 0;
    }
    delete userData;
    esContext->userData = nullptr;
}

void DepthTestingUpdateMatrix(ESContext *esContext, glm::vec3 transVec3){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    int angleXRotate = (userData->angleX  + 10) % 360;
    int angleYRotate = (userData->angleY + 10 ) % 360;
    float scale = 0.4f;
    float ratio = 1.0f * esContext->width / esContext->height;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleXRotate);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleYRotate);


    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 3), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scale, scale, scale));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, transVec3);


    userData->mMVPMatrix = Projection * View * Model;
}

void DepthTestingDraw(ESContext *esContext){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData || !userData->program) {
        return;
    }
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    glUseProgram(userData->program);
    glBindVertexArray(userData->vaoId);

    // 设置纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->imageTexture);
    glUniform1i(userData->textureLoc, 0);


    // 获取变换矩阵
    for(int i= 0; i < sizeof(transPos)/ sizeof(transPos[0]); i++){
        DepthTestingUpdateMatrix(esContext, transPos[i]);
        glUniformMatrix4fv(userData->mMVPMatrixLoc, 1, GL_FALSE, &userData->mMVPMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindVertexArray(GL_NONE);

}

void DepthTestingUpdateTransformMatrix(ESContext *esContext, GLfloat rotateX, GLfloat rotateY,
                                       GLfloat scaleX, GLfloat scaleY){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData) {
        return;
    }
    userData->angleX = rotateX;
    userData->angleY = rotateY;
}

}