#include <jni.h>
#include <android/native_window_jni.h>
#include "common/esUtil.h"

extern JNIEXPORT void JNICALL
Java_com_xucz_opengldemo_GLAPI_setSurface(
        JNIEnv *env,
        jclass thiz, jobject android_surface) {

    ANativeWindow *native_window = NULL;
    if (android_surface) {
        native_window = ANativeWindow_fromSurface(env, android_surface);
        if (!native_window) {
            ALOGE("%s: ANativeWindow_fromSurface: failed\n", __func__);
            return;
        }
    }

    ESContext esContext;
    memset(&esContext, 0, sizeof(struct ESContext));

    esContext.eglNativeDisplay = EGL_DEFAULT_DISPLAY;
    esContext.eglNativeWindow = native_window;
    esMain(&esContext);
    if (esContext.drawFunc != NULL) {
        esContext.drawFunc(&esContext);
        eglSwapBuffers(esContext.eglDisplay, esContext.eglSurface);
    }
}
