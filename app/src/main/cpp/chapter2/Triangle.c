//
// Created by xucz on 2019-06-22.
//

#include "../common/esUtil.h"

typedef struct {
    GLuint programObject;
} UserData;


GLuint LoadShader(GLenum type, const char *shaderSrc) {
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);
    if (shader == 0) {
        return 0;
    }

    glShaderSource(shader, 1, &shaderSrc, NULL);

    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char *infoLog = malloc(sizeof(char) * infoLen);
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            esLogMessage("Error compiling shader:\n%s\n", infoLog);

            free(infoLog);
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

int Init(ESContext *esContext) {
    UserData *userData = esContext->userData;

    char vShaderStr[] =
            "#version 300 es                           \n"
            "layout(location = 0) in vec4 vPosition;   \n"
            "void main()                               \n"
            "{                                         \n"
            "    gl_Position = vPosition;               \n"
            "}                                         \n";

    char fShaderStr[] =
            "#version 300 es                           \n"
            "precision mediump float;                  \n"
            "out vec4 fragColor;                       \n"
            "void main()                               \n"
            "{                                         \n"
            "    fragColor=vec4(0.0, 0.0, 1.0, 1.0);   \n"
            "}                                         \n";

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;

    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    programObject = glCreateProgram();
    if (programObject == 0) {
        return 0;
    }

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glLinkProgram(programObject);

    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            char *infoLog = malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            esLogMessage("Error linking program:\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteProgram(programObject);
        return 0;
    }
    userData->programObject = programObject;

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    return 1;
}

void Draw(ESContext *esContext) {
    GLfloat vVertices[] = {
            0.0f, 0.1f, 0.0f,
            -0.1f, -0.1f, 0.0f,
            0.0f, -0.2f, 0.0f
    };
    UserData *userData = esContext->userData;

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(userData->programObject);
    // 第1个参数：指定要修改的顶点属性的索引值
    // 第2个参数：指定每个顶点属性的组件数量。必须为1、2、3或者4。初始值为4。（如position是由3个（x,y,z）组成，而颜色是4个（r,g,b,a））
    // 第3个参数：指定数组中每个组件的数据类型。可用的符号常量有GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT,GL_UNSIGNED_SHORT, GL_FIXED, 和 GL_FLOAT，初始值为GL_FLOAT。
    // 第4个参数：指定当被访问时，固定点数据值是否应该被归一化（GL_TRUE）或者直接转换为固定点值（GL_FALSE）。
    // 第5个参数：指定连续顶点属性之间的偏移量。如果为0，那么顶点属性会被理解为：它们是紧密排列在一起的。初始值为0。
    // 第6个参数：指定第一个组件在数组的第一个顶点属性中的偏移量。该数组与GL_ARRAY_BUFFER绑定，储存于缓冲区中。初始值为0；
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);
    // 第1个参数：有三种取值
    //
    //          1.GL_TRIANGLES：每三个顶之间绘制三角形，之间不连接
    //
    //          2.GL_TRIANGLE_FAN：以V0V1V2,V0V2V3,V0V3V4，……的形式绘制三角形
    //
    //          3.GL_TRIANGLE_STRIP：顺序在每三个顶点之间均绘制三角形。这个方法可以保证从相同的方向上所有三角形均被绘制。以V0V1V2,V1V2V3,V2V3V4……的形式绘制三角形
    //
    // 第2个参数：从数组缓存中的哪一位开始绘制，一般都定义为0
    // 第3个参数：顶点的数量
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Shutdown(ESContext *esContext) {
    UserData *userData = esContext->userData;
    glDeleteProgram(userData->programObject);
}

int esMain(ESContext *esContext) {
    esContext->userData = malloc(sizeof(UserData));
    esCreateWindow(esContext, "Hello Triangle", 320, 240, ES_WINDOW_RGB);
    if (!Init(esContext)) {
        return GL_FALSE;
    }
    esRegisterShutdownFunc(esContext, Shutdown);
    esRegisterDrawFunc(esContext, Draw);
    return GL_TRUE;
}

