//
// Created by xucz on 2020/7/26.
//

#include "FBO.h"

typedef struct __UserData {
    GLuint imageTextureId;

    GLuint fboId;
    GLuint fboTextureId;

    GLuint fboProgramId;
    GLuint fboTextureLoc;
    GLuint fbovbos[2];
    GLuint fbovao;

    GLuint screenProgramId;
    GLuint screenTextureLoc;
    GLuint screenvbos[2];
    GLuint screenvao;

} UserData;

void FBOInit(ESContext *esContext) {
    UserData *userData = esContext->userData;
    if (userData == NULL) {
        userData = malloc(sizeof(UserData));
        memset(userData, 0, sizeof(UserData));
        esContext->userData = userData;
    }

    // 创建一个纹理，以接收外部图像数据
    GLuint imageTexture;
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    userData->imageTextureId = imageTexture;

    // 创建离屏FBO
    GLuint fboId, fboTexture;
    glGenFramebuffers(1, &fboId);
    //   创建一个纹理，用于连接FBO
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    //    连接纹理
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    // !! 绑定的是fboTexture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);
    // !! 必须先分配内存
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, esContext->width, esContext->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    //    检查FBO
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ){
        // error
        return;
    }
    //    解绑
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

    userData->fboId = fboId;
    userData->fboTextureId = fboTexture;

    // 图像数据到FBO的着色器程序 及 VAO
    //      编译着色器，输出灰度图
    char vFboShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec2 a_texCoord;   \n"
            "out vec2 v_texCoord;                       \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = a_position;               \n"
            "   v_texCoord = a_texCoord;                \n"
            "}                                          \n";

    char fFboShaderStr[] =
            "#version 300 es                            \n"
            "precision mediump float;                   \n"
            "in vec2 v_texCoord;                        \n"
            "layout(location = 0) out vec4 outColor;    \n"
            "uniform sampler2D s_TextureMap;            \n"
            "void main()                                \n"
            "{                                          \n"
            "    vec4 tempColor = texture(s_TextureMap, v_texCoord);\n"
            "    float luminance = tempColor.r * 0.299 + tempColor.g * 0.587 + tempColor.b * 0.114;\n"
            "    outColor = vec4(vec3(luminance), tempColor.a);\n"
            "}";
    GLuint fFboProgram = esLoadProgram(vFboShaderStr, fFboShaderStr);
    if(fFboProgram == 0){
        return;
    }
    userData->fboProgramId = fFboProgram;
    userData->fboTextureLoc = glGetUniformLocation(fFboProgram, "s_TextureMap");
    //     确定纹理坐标和顶点坐标，fbo纹理坐标原点在左下角
    GLfloat fbovertices[] = {
            -1.0f, 1.0f, 0, // v1
            0, 1.0f,  // t1
            -1.0f, -1.0f, 0, // v2
            0, 0, // t2
            1.0f, -1.0f, 0, // v3
            1.0f, 0, // t3
            1.0f, 1.0f, 0, //v4
            1.0f, 1.0f // t4
    };
    // !!图元顺序使用GLushort
    GLushort fboindices[] = {0, 1, 2, 0, 2, 3};

    //     创建VBO保存数组数据到GPU中
    glGenBuffers(2, userData->fbovbos);
    glBindBuffer(GL_ARRAY_BUFFER, userData->fbovbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbovertices), fbovertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->fbovbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fboindices), fboindices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

    //      创建VAO并记录VBO操作
    glGenVertexArrays(1, &userData->fbovao);
    glBindVertexArray(userData->fbovao);
    glBindBuffer(GL_ARRAY_BUFFER, userData->fbovbos[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5,
            (const void*) (sizeof(GL_FLOAT) * 3));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->fbovbos[1]);
    glBindVertexArray(GL_NONE);



    // FBO到屏幕的着色器程序 及 VAO
    //      编译着色器
    char vScreenShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec2 a_texCoord;   \n"
            "out vec2 v_texCoord;                       \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = a_position;               \n"
            "   v_texCoord = a_texCoord;                \n"
            "}                                          \n";
    char fScreenShaderStr[] =
            "#version 300 es                                    \n"
            "precision mediump float;                           \n"
            "in vec2 v_texCoord;                                \n"
            "layout(location = 0) out vec4 outColor;            \n"
            "uniform sampler2D s_TextureMap;                    \n"
            "void main()                                        \n"
            "{                                                  \n"
            "    outColor = texture(s_TextureMap, v_texCoord);  \n"
            "}";

    GLuint screenProgram = esLoadProgram(vScreenShaderStr, fScreenShaderStr);
    if(screenProgram == 0){
        return;
    }
    userData->screenProgramId = screenProgram;
    userData->screenTextureLoc = glGetUniformLocation(screenProgram, "s_TextureMap");
    //      确定纹理坐标和顶点坐标
    GLfloat screenvertices[] = {
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
    //      保存数组到VBO中
    glGenBuffers(2, userData->screenvbos);
    glBindBuffer(GL_ARRAY_BUFFER, userData->screenvbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenvertices), screenvertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->screenvbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(screenindices), screenindices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
    //      生成VAO
    glGenVertexArrays(1, &userData->screenvao);
    glBindVertexArray(userData->screenvao);
    glBindBuffer(GL_ARRAY_BUFFER, userData->screenvbos[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, (const void*)(sizeof(GL_FLOAT) * 3));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->screenvbos[1]);
    glBindVertexArray(GL_NONE);


}

void FBODraw(ESContext *esContext) {
    UserData *userData = esContext->userData;
    if(userData == NULL){
        return;
    }

    // 绘制Image到图像纹理
    NativeImage *image = esContext->imageData;
    if(image == NULL){
        return;
    }
    if(image->format != IMAGE_FORMAT_RGBA){
        return;
    }
    glBindTexture(GL_TEXTURE_2D, userData->imageTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, image->ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 将imageTextureId上的图像渲染到FBO上
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, userData->fboId);
    glUseProgram(userData->fboProgramId);
    glBindVertexArray(userData->fbovao);
    //     将imageTextureId设置给着色器
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->imageTextureId);
    glUniform1i(userData->fboTextureLoc, 0);
    //      绘制
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    GO_CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindVertexArray(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

    // 将FBO中的图像渲染到屏幕上
    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(userData->screenProgramId);
    glBindVertexArray(userData->screenvao);
    //      将FBO的纹理id设置给着色器
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->fboTextureId);
    glUniform1i(userData->screenTextureLoc, 0);
    //      绘制
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    GO_CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindVertexArray(GL_NONE);

}

void FBOUninit(ESContext *esContext) {
    UserData *userData = esContext->userData;
    if(userData == NULL){
        return;
    }
    if(userData->imageTextureId){
        glDeleteTextures(1, &userData->imageTextureId);
    }
    if(userData->fboId){
        glDeleteFramebuffers(1, &userData->fboId);
    }
    if(userData->fboTextureId){
        glDeleteTextures(1, &userData->fboTextureId);
    }
    if(userData->fboProgramId){
        glDeleteProgram(userData->fboProgramId);
    }
    if(userData->fbovbos[0]){
        glDeleteBuffers(2, userData->fbovbos);
    }
    if(userData->fbovao){
        glDeleteVertexArrays(1, &userData->fbovao);
    }
    if(userData->screenProgramId){
        glDeleteProgram(userData->screenProgramId);
    }
    if(userData->screenvbos[0]){
        glDeleteBuffers(2, userData->screenvbos);
    }
    if(userData->screenvao){
        glDeleteVertexArrays(1, &userData->screenvao);
    }
    free(userData);
    esContext->userData = NULL;
}


