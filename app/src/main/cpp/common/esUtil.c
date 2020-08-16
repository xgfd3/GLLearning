//
// Created by xucz on 2019-06-22.
//
#include "esUtil.h"

void esLogMessage(const char *formatStr, ...) {
    va_list params;
    char buf[BUFSIZ];

    va_start (params, formatStr);
    vsprintf(buf, formatStr, params);

    __android_log_print(ANDROID_LOG_INFO, "esUtil", "%s", buf);
}

GLboolean esCreateWindow(ESContext *esContext, GLuint flags, GLint width, GLint height) {
    EGLConfig config;
    EGLint majorVersion;
    EGLint minorVersion;
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};

    if (esContext == NULL) {
        return GL_FALSE;
    }


    // 1. 获取EGLDisplay对象，建立与本地窗口系统的连接
    esContext->eglDisplay = eglGetDisplay(esContext->eglNativeDisplay);
    if (esContext->eglDisplay == EGL_NO_DISPLAY) {
        return GL_FALSE;
    }

    // 2. Initialize EGL
    if (!eglInitialize(esContext->eglDisplay, &majorVersion, &minorVersion)) {
        return GL_FALSE;
    }

    // 3. 获取 EGLConfig 对象，确定渲染表面的配置信息
    EGLint numConfigs = 0;
    EGLint attribList[] =
            {
                    EGL_RED_SIZE, 8,
                    EGL_GREEN_SIZE, 8,
                    EGL_BLUE_SIZE, 8,
                    EGL_ALPHA_SIZE, (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
                    EGL_DEPTH_SIZE, (flags & ES_WINDOW_DEPTH) ? 16 : EGL_DONT_CARE,
                    EGL_STENCIL_SIZE, (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
                    EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
                    // if EGL_KHR_create_context extension is supported, then we will use
                    // EGL_OPENGL_ES3_BIT_KHR instead of EGL_OPENGL_ES2_BIT in the attribute list
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                    EGL_SURFACE_TYPE,
                    (flags & ES_WINDOW_PBUFFER) ? EGL_PBUFFER_BIT : EGL_WINDOW_BIT,
                    EGL_NONE
            };
    if (!eglChooseConfig(esContext->eglDisplay, attribList, &config, 1, &numConfigs)) {
        return GL_FALSE;
    }

    if (numConfigs < 1) {
        return GL_FALSE;
    }

    // 4. 创建渲染表面 EGLSurface
    if (flags & ES_WINDOW_PBUFFER) {
        // 使用eglCreatePbufferSurface 创建屏幕外渲染区域
        const EGLint surfaceAttr[] = {
                EGL_WIDTH, 1,
                EGL_HEIGHT, 1,
                EGL_NONE
        };

        esContext->eglSurface = eglCreatePbufferSurface(esContext->eglDisplay, config, surfaceAttr);
    } else {
        // 使用eglCreateWindowSurface 创建屏幕上渲染区域
        if (esContext->eglNativeWindow) {

            // For Android, get the width/height from the window rather than what the
            // application requested.
            esContext->width = ANativeWindow_getWidth(esContext->eglNativeWindow);
            esContext->height = ANativeWindow_getHeight(esContext->eglNativeWindow);

            // For Android, need to get the EGL_NATIVE_VISUAL_ID and set it using ANativeWindow_setBuffersGeometry
            EGLint format = 0;
            eglGetConfigAttrib(esContext->eglDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
            ANativeWindow_setBuffersGeometry(esContext->eglNativeWindow, 0, 0, format);

            // Create a surface
            esContext->eglSurface = eglCreateWindowSurface(esContext->eglDisplay, config,
                                                           esContext->eglNativeWindow, NULL);
        }
    }

    if (esContext->eglSurface == EGL_NO_SURFACE) {
        return GL_FALSE;
    }

    if (esContext->width <= 0) {
        esContext->width = width;
    }
    if (esContext->height <= 0) {
        esContext->height = height;
    }


    // 5. 创建渲染上下文 EGLContext
    esContext->eglContext = eglCreateContext(esContext->eglDisplay, config,
                                             EGL_NO_CONTEXT, contextAttribs);

    if (esContext->eglContext == EGL_NO_CONTEXT) {
        return GL_FALSE;
    }

    // 6. 绑定上下文
    if (!eglMakeCurrent(esContext->eglDisplay, esContext->eglSurface,
                        esContext->eglSurface, esContext->eglContext)) {
        return GL_FALSE;
    }

    return GL_TRUE;
}

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

GLuint esLoadProgram(const char *vertShaderSrc, const char *fragShaderSrc) {
    return esLoadProgram2(vertShaderSrc, fragShaderSrc, NULL);
}

GLuint
esLoadProgram2(const char *vertShaderSrc, const char *fragShaderSrc, void (*beforeLink)(GLuint)) {
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;

    vertexShader = LoadShader(GL_VERTEX_SHADER, vertShaderSrc);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragShaderSrc);

    programObject = glCreateProgram();
    if (programObject == 0) {
        return 0;
    }

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    if (beforeLink) {
        beforeLink(programObject);
    }

    glLinkProgram(programObject);
    GO_CHECK_GL_ERROR();

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
    return programObject;
}


long esGetCurrClockTimeNs() {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_nsec;
}
