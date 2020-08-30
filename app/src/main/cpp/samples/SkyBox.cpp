//
// Created by xucz on 2020/8/16.
//

#include "SkyBox.h"

extern "C" {

typedef struct __UserData{

    GLuint skyProgram;
    GLuint skyMVPMatrixLoc;
    GLuint skyTextureLoc;
    
    GLuint skyVboId;
    GLuint skyVaoId;

    GLuint cubeProgram;
    GLuint cubeMVPMatrixLoc;
    GLuint cubeModelMatrixLoc;
    GLuint cubeTextureLoc;
    GLuint cubeCameraPosLoc;

    GLuint cubeVboId;
    GLuint cubeVaoId;
    
    GLuint imageTexture;

    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;

    int angleX;
    int angleY;


} UserData;

void SkyBoxInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if( !userData ){
        userData = new UserData();
        esContext->userData = userData;
    }

    // 天空着色器
    char skyVShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "layout(location = 0) in vec3 a_position;\n"
            "uniform mat4 u_MVPMatrix;\n"
            "out vec3 v_texCoord;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_MVPMatrix * vec4(a_position, 1.0);\n"
            "    v_texCoord = a_position;\n"
            "}"
            ;

    char skyFShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec3 v_texCoord;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform samplerCube s_SkyBox;\n"
            "void main()\n"
            "{\n"
            "    outColor = texture(s_SkyBox, v_texCoord);\n"
            "}"
            ;

    //       编译着色器
    GLuint skyProgram = esLoadProgram(skyVShaderStr, skyFShaderStr);
    if(!skyProgram){
        return;
    }
    userData->skyProgram = skyProgram;
    userData->skyMVPMatrixLoc = glGetUniformLocation(skyProgram, "u_MVPMatrix");
    userData->skyTextureLoc = glGetUniformLocation(skyProgram, "s_SkyBox");

    //       确定顶点坐标
    GLfloat skyVertices[] = {
            // Positions
            -2.0f,  2.0f, -2.0f,
            -2.0f, -2.0f, -2.0f,
             2.0f, -2.0f, -2.0f,
             2.0f, -2.0f, -2.0f,
             2.0f,  2.0f, -2.0f,
            -2.0f,  2.0f, -2.0f,

            -2.0f, -2.0f,  2.0f,
            -2.0f, -2.0f, -2.0f,
            -2.0f,  2.0f, -2.0f,
            -2.0f,  2.0f, -2.0f,
            -2.0f,  2.0f,  2.0f,
            -2.0f, -2.0f,  2.0f,

             2.0f, -2.0f, -2.0f,
             2.0f, -2.0f,  2.0f,
             2.0f,  2.0f,  2.0f,
             2.0f,  2.0f,  2.0f,
             2.0f,  2.0f, -2.0f,
             2.0f, -2.0f, -2.0f,

            -2.0f, -2.0f,  2.0f,
            -2.0f,  2.0f,  2.0f,
             2.0f,  2.0f,  2.0f,
             2.0f,  2.0f,  2.0f,
             2.0f, -2.0f,  2.0f,
            -2.0f, -2.0f,  2.0f,

            -2.0f,  2.0f, -2.0f,
             2.0f,  2.0f, -2.0f,
             2.0f,  2.0f,  2.0f,
             2.0f,  2.0f,  2.0f,
            -2.0f,  2.0f,  2.0f,
            -2.0f,  2.0f, -2.0f,

            -2.0f, -2.0f, -2.0f,
            -2.0f, -2.0f,  2.0f,
             2.0f, -2.0f, -2.0f,
             2.0f, -2.0f, -2.0f,
            -2.0f, -2.0f,  2.0f,
             2.0f, -2.0f,  2.0f
    };
    
    //     使用VBO缓存数据
    glGenBuffers(1, &userData->skyVboId);
    glBindBuffer(GL_ARRAY_BUFFER, userData->skyVboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    
    //     使用VAO记录赋值过程
    glGenVertexArrays(1, &userData->skyVaoId);
    glBindVertexArray(userData->skyVaoId);
    
    glBindBuffer(GL_ARRAY_BUFFER, userData->skyVboId);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    // 盒子着色器
    char cubeVShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "layout(location = 0) in vec3 a_position;\n"
            "layout(location = 1) in vec3 a_normal;\n"
            "uniform mat4 u_MVPMatrix;\n"
            "uniform mat4 u_ModelMatrix;\n"
            "out vec3 v_texCoord;\n"
            "out vec3 v_normal;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_MVPMatrix * vec4(a_position, 1.0);\n"
            "    v_normal = mat3(transpose(inverse(u_ModelMatrix))) * a_normal;\n"
            "    v_texCoord = vec3(u_ModelMatrix * vec4(a_position, 1.0));\n"
            "}"
            ;

    char cubeFShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec3 v_texCoord;\n"
            "in vec3 v_normal;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform samplerCube s_SkyBox;\n"
            "uniform vec3 u_cameraPos;\n"
            "void main()\n"
            "{\n"
            "    float ratio = 1.00 / 1.52;\n"
            "    vec3 I = normalize(v_texCoord - u_cameraPos);\n"
            "    // 反射  \n"
            "    vec3 R = reflect(I, normalize(v_normal));\n"
            "    // 折射  \n"
            "    // vec3 R = refract(I, normalize(v_normal), ratio);\n"
            "    outColor = texture(s_SkyBox, R);\n"
            "}"
            ;

    
    GLuint cubeProgram = esLoadProgram(cubeVShaderStr, cubeFShaderStr);
    if(!cubeProgram){
        return;
    }
    userData->cubeProgram = cubeProgram;
    userData->cubeMVPMatrixLoc = glGetUniformLocation(cubeProgram, "u_MVPMatrix");
    userData->cubeModelMatrixLoc = glGetUniformLocation(cubeProgram, "u_ModelMatrix");
    userData->cubeTextureLoc = glGetUniformLocation(cubeProgram, "s_SkyBox");
    userData->cubeCameraPosLoc = glGetUniformLocation(cubeProgram, "u_cameraPos");

    //       确定顶点坐标，和法向量
    GLfloat cubeVertices[] = {
            //position           //normal
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f, 0.5f,   0.0f,  0.0f,  1.0f,
             0.5f, -0.5f, 0.5f,   0.0f,  0.0f,  1.0f,
             0.5f,  0.5f, 0.5f,   0.0f,  0.0f,  1.0f,
             0.5f,  0.5f, 0.5f,   0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f, 0.5f,   0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, 0.5f,   0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

            -0.5f, 0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
             0.5f, 0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
             0.5f, 0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
             0.5f, 0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
            -0.5f, 0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
            -0.5f, 0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
    };
    
    //     使用VBO缓存数据
    glGenBuffers(1, &userData->cubeVboId);
    glBindBuffer(GL_ARRAY_BUFFER, userData->cubeVboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    
    //     使用VAO记录缓存过程
    glGenVertexArrays(1, &userData->cubeVaoId);
    glBindVertexArray(userData->cubeVaoId);
    
    glBindBuffer(GL_ARRAY_BUFFER, userData->cubeVboId);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    
    glBindVertexArray(GL_NONE);
    
    // 生成3D纹理，并加载3D图片
    glGenTextures(1, &userData->imageTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, userData->imageTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto **images = reinterpret_cast<NativeImage **>(esContext->imageData);
    for(int i= 0; i< 6; i++){
        NativeImage *image = images[i];
        if(image && image->format == IMAGE_FORMAT_RGBA){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                         GL_RGBA, image->width, image->height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, image->ppPlane[0]);
        }
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);
}

void SkyBoxUnInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if( !userData ){
        return;
    }
    if(userData->skyProgram){
        glDeleteProgram(userData->skyProgram);
        userData->skyProgram = GL_NONE;
    }
    if(userData->skyVboId){
        glDeleteBuffers(1, &userData->skyVboId);
        userData->skyVboId = GL_NONE;
    }
    if(userData->skyVaoId){
        glDeleteVertexArrays(1, &userData->skyVaoId);
        userData->skyVaoId = GL_NONE;
    }
    if(userData->cubeProgram){
        glDeleteProgram(userData->cubeProgram);
        userData->cubeProgram = GL_NONE;
    }
    if(userData->cubeVboId){
        glDeleteBuffers(1, &userData->cubeVboId);
        userData->cubeVboId = GL_NONE;
    }
    if(userData->cubeVaoId){
        glDeleteVertexArrays(1, &userData->cubeVaoId);
    }
    if(userData->imageTexture){
        glDeleteTextures(1, &userData->imageTexture);
        userData->imageTexture = GL_NONE;
    }
    delete userData;
    esContext->userData = nullptr;
}

void SkyBoxUpdateMatrix(ESContext *esContext, float scale){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if( !userData ){
        return;
    }
    int angleX = userData->angleX % 360;
    int angleY = userData->angleY % 360;
    float ratio = 1.0f * esContext->width / esContext->height;

    // 转成弧度角
    float radiansX = MATH_PI / 180.f * angleX;
    float radiansY = MATH_PI / 180.f * angleY;

    glm::mat4 Projection = glm::perspective(
            45.f, ratio, 0.1f, 100.f);

    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 1.8),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
            );

    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scale, scale, scale));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.f, 0.f, 0.f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.f, 1.f, 0.f));
    Model = glm::translate(Model, glm::vec3(0.f, 0.f, 0.f));

    userData->m_ModelMatrix = Model;
    userData->m_MVPMatrix = Projection * View * Model;
}

void SkyBoxDraw(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if( !userData || !userData->skyProgram || !userData->cubeProgram){
        return;
    }

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(1.f, 1.f, 1.f, 0.f);

    glEnable(GL_DEPTH_TEST);

    // 画天空
    glUseProgram(userData->skyProgram);
    glBindVertexArray(userData->skyVaoId);

    //      设置变换矩阵
    SkyBoxUpdateMatrix(esContext, 1.0f);
    glUniformMatrix4fv(userData->skyMVPMatrixLoc, 1, GL_FALSE, &userData->m_MVPMatrix[0][0]);

    //      设置纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, userData->imageTexture);
    glUniform1i(userData->skyTextureLoc, 0);

    //      绘制
    glDrawArrays(GL_TRIANGLES, 0, 36);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);

    // 画盒子
    glUseProgram(userData->cubeProgram);
    glBindVertexArray(userData->cubeVaoId);

    //      设置变换矩阵
    SkyBoxUpdateMatrix(esContext, 0.4f);
    glUniformMatrix4fv(userData->cubeMVPMatrixLoc, 1, GL_FALSE, &userData->m_MVPMatrix[0][0]);
    glUniformMatrix4fv(userData->cubeModelMatrixLoc, 1, GL_FALSE, &userData->m_ModelMatrix[0][0]);

    //      设置纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, userData->imageTexture);
    glUniform1i(userData->cubeTextureLoc, 0);

    //      设置相机位置
    glUniform3f(userData->cubeCameraPosLoc, 0.f, 0.f, 1.8f);

    //      绘制
    glDrawArrays(GL_TRIANGLES, 0, 36);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);


}

void SkyBoxUpdateTransformMatrix(ESContext *esContext, GLfloat rotateX, GLfloat rotateY,
                                 GLfloat scaleX, GLfloat scaleY) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if( !userData ){
        return;
    }

    userData->angleX = rotateX;
    userData->angleY = rotateY;
}

}