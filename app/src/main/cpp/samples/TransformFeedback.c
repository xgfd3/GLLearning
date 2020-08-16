//
// Created by xucz on 2020/8/7.
//

#include "TransformFeedback.h"

typedef struct __UserData{
    GLuint imageTexture;

    GLuint program;
    GLuint textureLoc;
    GLuint vboIds[1];
    GLuint vaoId;

    GLuint tfBufferId;
    GLuint tfId;
} UserData;

void TransformFeedbackBeforeLink(GLuint program){
    // 1. 设置变换反馈变量
    GLchar const *veryings[] = {"outPos", "outTex"};
    glTransformFeedbackVaryings(program, sizeof(veryings)/sizeof(veryings[0]),
            veryings, GL_INTERLEAVED_ATTRIBS);
}

void TransformFeedbackInit(ESContext *esContext){
    UserData *userData = esContext->userData;
    if(!userData){
        userData = malloc(sizeof(UserData));
        memset(userData, 0, sizeof(UserData));
        esContext->userData = userData;
    }

    // 准备着色器
    char vShaderStr[] =
            "#version 300 es    \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec2 a_texCoord;   \n"
            "out vec2 v_texCoord;   \n"
            "out vec3 outPos;       \n"
            "out vec2 outTex;       \n"
            "void main()            \n"
            "{                      \n"
            "gl_Position = a_position;   \n"
            "v_texCoord = a_texCoord;    \n"
            "outPos = vec3(a_position) * 3.0;  \n"
            "outTex = a_texCoord * 3.0;   \n"
            "}";

    char fShaderStr[] =
            "#version 300 es                           \n"
            "precision mediump float;                           \n"
            "in vec2 v_texCoord;   \n"
            "layout(location = 0) out vec4 outColor;  \n"
            "uniform sampler2D s_TextureMap;  \n"
            "void main()                               \n"
            "{                                         \n"
            "    outColor = texture(s_TextureMap, v_texCoord);               \n"
            "}                                         \n";

    // 编译前设置变换反馈对象，然后编译
    userData->program = esLoadProgram2(vShaderStr, fShaderStr, TransformFeedbackBeforeLink);
    GO_CHECK_GL_ERROR();
    userData->textureLoc = glGetUniformLocation(userData->program, "s_TextureMap");

    // 使用VBO保存顶点坐标和纹理坐标
    GLfloat coords[] = {
            -1.0f, -0.5f, 0,
            0.0f, 1.0f,
            1.0f, -0.5f, 0,
            1.0f, 1.0f,
            -1.0f, 0.5f, 0,
            0.0f, 0.0f,

            1.0f, -0.5f, 0,
            1.0f, 1.0f,
            1.0f, 0.5f, 0,
            1.0f, 0.0f,
            -1.0f, 0.5f, 0.0f,
            0.0f, 0.0f
    };

    glGenBuffers(1, userData->vboIds);
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    // 使用VAO记录坐标赋值
    glGenVertexArrays(1, &userData->vaoId);
    glBindVertexArray(userData->vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    // !! 注意最后一个参数的偏移量，单位是Byte
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3* sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindVertexArray(GL_NONE);


    // 2. 创建变换反馈缓存
    glGenBuffers(1, &userData->tfBufferId);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, userData->tfBufferId);
    // !! 第二个参数，size是指绘制后所有的数据集合，不只是一个的
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, (3 + 2) * 6 *  sizeof(GLfloat),
            NULL, GL_STATIC_READ);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_NONE);

    // 3. 生成变换反馈对象，并绑定缓存区
    glGenTransformFeedbacks(1, &userData->tfId);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, userData->tfId);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, userData->tfBufferId);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, GL_NONE);

    // 创建纹理来加载图片
    glGenTextures(1, &userData->imageTexture);
    glBindTexture(GL_TEXTURE_2D, userData->imageTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    NativeImage *imageData = esContext->imageData[0];
    if(imageData && imageData->format == IMAGE_FORMAT_RGBA){
        glTexImage2D(GL_TEXTURE_2D, 0,
                GL_RGBA, imageData->width, imageData->height,
                0, GL_RGBA, GL_UNSIGNED_BYTE, imageData->ppPlane[0]);
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    GO_CHECK_GL_ERROR();
}

void TransformFeedbackUninit(ESContext *esContext){
    UserData *userData = esContext->userData;
    if(!userData){
        return;
    }
    if(userData->imageTexture){
        glDeleteTextures(1, &userData->imageTexture);
        userData->imageTexture = 0;
    }
    if(userData->vaoId){
        glDeleteVertexArrays(1, &userData->vaoId);
        userData->vaoId = 0;
    }
    if(userData->vboIds[0]){
        glDeleteBuffers(1, userData->vboIds);
        userData->vboIds[0] = 0;
    }
    if(userData->program){
        glDeleteProgram(userData->program);
        userData->program = 0;
    }
    if(userData->tfBufferId){
        glDeleteBuffers(1, &userData->tfBufferId);
        userData->tfBufferId = 0;
    }
    if(userData->tfId){
        glDeleteTransformFeedbacks(1, &userData->tfId);
        userData->tfId = 0;
    }
    free(userData);
    esContext->userData = NULL;
}

void TransformFeedbackDraw(ESContext *esContext){
    UserData *userData = esContext->userData;
    if(!userData){
        return;
    }
    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(userData->program);
    glBindVertexArray(userData->vaoId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->imageTexture);
    glUniform1i(userData->textureLoc, 0);


    // 4-1. 启动变换反馈
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, userData->tfId);
    glBeginTransformFeedback(GL_TRIANGLES);
    //      绘制
    glDrawArrays(GL_TRIANGLES, 0, 6);
    GO_CHECK_GL_ERROR();
    // 4-2. 结束变换反馈
    glEndTransformFeedback();
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, GL_NONE);
    glBindVertexArray(GL_NONE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 5. 读取变换反馈缓冲区数据
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, userData->tfBufferId);
    void *data = glMapBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0, (3 + 2) * 6 * sizeof(GLfloat),
                                     GL_MAP_READ_BIT);
    if(data){
        float *rawData = (float *)data;
        for(int i=0; i < 6; i++){
            LOGCATE("TransformFeedbackDraw read feedback buffer outPos[%d] =[%f, %f, %f], outText[%d] = [%f, %f]",
                    i, rawData[i*5], rawData[i*5 + 1], rawData[i*5 + 2],
                    i, rawData[i*5 + 3], rawData[i*5 + 4]);
        }
    }
    glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
}

