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

GLboolean esCreateWindow(struct ESContext *esContext, const char *title, GLint width, GLint height,
                         GLuint flags) {
    EGLConfig config;
    EGLint majorVersion;
    EGLint minorVersion;
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};

    if (esContext == NULL) {
        return GL_FALSE;
    }

    // For Android, get the width/height from the window rather than what the
    // application requested.
    esContext->width = ANativeWindow_getWidth(esContext->eglNativeWindow);
    esContext->height = ANativeWindow_getHeight(esContext->eglNativeWindow);


    esContext->eglDisplay = eglGetDisplay(esContext->eglNativeDisplay);
    if (esContext->eglDisplay == EGL_NO_DISPLAY) {
        return GL_FALSE;
    }

    // Initialize EGL
    if (!eglInitialize(esContext->eglDisplay, &majorVersion, &minorVersion)) {
        return GL_FALSE;
    }

    {
        EGLint numConfigs = 0;
        EGLint attribList[] =
                {
                        EGL_RED_SIZE, 5,
                        EGL_GREEN_SIZE, 6,
                        EGL_BLUE_SIZE, 5,
                        EGL_ALPHA_SIZE, (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
                        EGL_DEPTH_SIZE, (flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
                        EGL_STENCIL_SIZE, (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
                        EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
                        // if EGL_KHR_create_context extension is supported, then we will use
                        // EGL_OPENGL_ES3_BIT_KHR instead of EGL_OPENGL_ES2_BIT in the attribute list
                        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                        EGL_NONE
                };

        // Choose config
        if (!eglChooseConfig(esContext->eglDisplay, attribList, &config, 1, &numConfigs)) {
            return GL_FALSE;
        }

        if (numConfigs < 1) {
            return GL_FALSE;
        }
    }


    // For Android, need to get the EGL_NATIVE_VISUAL_ID and set it using ANativeWindow_setBuffersGeometry
    {
        EGLint format = 0;
        eglGetConfigAttrib(esContext->eglDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
        ANativeWindow_setBuffersGeometry(esContext->eglNativeWindow, 0, 0, format);
    }

    // Create a surface
    esContext->eglSurface = eglCreateWindowSurface(esContext->eglDisplay, config,
                                                   esContext->eglNativeWindow, NULL);

    if (esContext->eglSurface == EGL_NO_SURFACE) {
        return GL_FALSE;
    }

    // Create a GL context
    esContext->eglContext = eglCreateContext(esContext->eglDisplay, config,
                                             EGL_NO_CONTEXT, contextAttribs);

    if (esContext->eglContext == EGL_NO_CONTEXT) {
        return GL_FALSE;
    }

    // Make the context current
    if (!eglMakeCurrent(esContext->eglDisplay, esContext->eglSurface,
                        esContext->eglSurface, esContext->eglContext)) {
        return GL_FALSE;
    }

    return GL_TRUE;
}


void
esRegisterShutdownFunc(struct ESContext *esContext, void ( *shutdownFunc )(struct ESContext *)) {
    esContext->shutdownFunc = shutdownFunc;
}

void esRegisterDrawFunc(struct ESContext *esContext, void ( *drawFunc )(struct ESContext *)) {
    esContext->drawFunc = drawFunc;
}