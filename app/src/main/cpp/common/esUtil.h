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


#define ALOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"GLAPI_native", __VA_ARGS__)
#define ALOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "esUtil", __VA_ARGS__))

typedef struct ESContext {
    /// Put platform specific data here
    void *platformData;

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

    /// Callbacks
    void (  *drawFunc )(struct ESContext *);

    void (  *shutdownFunc )(struct ESContext *);

    void (  *keyFunc )(struct ESContext *, unsigned char, int, int);

    void (  *updateFunc )(struct ESContext *, float deltaTime);
} ESContext;


void esLogMessage(const char *formatStr, ...);

GLboolean esCreateWindow(ESContext *esContext, const char *title, GLint width, GLint height,
                         GLuint flags);

GLuint esLoadProgram ( const char *vertShaderSrc, const char *fragShaderSrc );

void esRegisterShutdownFunc(ESContext *esContext, void ( *shutdownFunc )(ESContext *));

void esRegisterDrawFunc(ESContext *esContext, void ( *drawFunc )(ESContext *));

long esGetCurrClockTimeNs();

int esMain(ESContext *esContext);

#endif //ANDROIDOPENGL3DEMO_ESUTIL_H
