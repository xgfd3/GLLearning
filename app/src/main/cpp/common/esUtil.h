//
// Created by xucz on 2019-06-22.
//

#ifndef ANDROIDOPENGL3DEMO_ESUTIL_H
#define ANDROIDOPENGL3DEMO_ESUTIL_H

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <android/log.h>
#include <time.h>
#include <android/native_window.h>

/// esCreateWindow flag - RGB color buffer
#define ES_WINDOW_RGB           0
/// esCreateWindow flag - ALPHA color buffer
#define ES_WINDOW_ALPHA         1
/// esCreateWindow flag - depth buffer
#define ES_WINDOW_DEPTH         2
/// esCreateWindow flag - stencil buffer
#define ES_WINDOW_STENCIL       4
/// esCreateWindow flat - multi-sample buffer
#define ES_WINDOW_MULTISAMPLE   8

#define LOG_TAG "AndroidOpenGL3Demo"

#define ALOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)
#define ALOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGCATE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define GO_CHECK_GL_ERROR(...)  \
{                               \
    int error = glGetError();   \
    if( error!= 0) LOGCATE("CHECK_GL_ERROR %s glGetError = %d, line = %d, ",  __FUNCTION__, error, __LINE__); \
}

typedef struct __ESContext {
    /// Put Image data here
    void *imageData;

    /// Put your user data here...
    void *userData;

    /// Window width
    GLint width;

    /// Window height
    GLint height;

    /// Display handle
    EGLNativeDisplayType eglNativeDisplay;

    /// Window handle
    EGLNativeWindowType eglNativeWindow;

    /// EGL display
    EGLDisplay eglDisplay;

    /// EGL context
    EGLContext eglContext;

    /// EGL surface
    EGLSurface eglSurface;

    // Callbacks what
    int cb_what;

    /// Callbacks
    void (  *initFunc )(struct __ESContext *);

    void (  *drawFunc )(struct __ESContext *);

    void (  *uninitFunc )(struct __ESContext *);

} ESContext;


void esLogMessage(const char *formatStr, ...);

GLboolean esCreateWindow(ESContext *esContext, const char *title, GLint width, GLint height,
                         GLuint flags);

GLuint esLoadProgram ( const char *vertShaderSrc, const char *fragShaderSrc );


long esGetCurrClockTimeNs();

#endif //ANDROIDOPENGL3DEMO_ESUTIL_H
