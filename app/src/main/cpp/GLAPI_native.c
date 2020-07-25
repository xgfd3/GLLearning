#include <jni.h>
#include <android/native_window_jni.h>
#include "GLAPI_native.h"

static jlong initGLEnv(JNIEnv *env,jobject thiz, jobject android_surface,
        jint width, jint height) {

    ANativeWindow *native_window = NULL;
    if (android_surface) {
        native_window = ANativeWindow_fromSurface(env, android_surface);
        if (!native_window) {
            ALOGE("%s: ANativeWindow_fromSurface: failed\n", __func__);
            return 0;
        }
    }

    ESContext *esContext;
    esContext = malloc(sizeof(struct ESContext));
    memset(esContext, 0, sizeof(struct ESContext));

    esContext->eglNativeDisplay = EGL_DEFAULT_DISPLAY;
    esContext->eglNativeWindow = native_window;

    esCreateWindow(esContext, "Hello Triangle", width, height, ES_WINDOW_RGB);


    return (jlong)esContext;
}

static void uninitGLEnv(JNIEnv *env,jobject thiz, jlong esContext){
    ESContext* _esContext = (ESContext*)esContext;
    if(_esContext == NULL){
        return;
    }

    if (_esContext->uninitFunc != NULL) {
        _esContext->uninitFunc(_esContext);
    }

    free(_esContext);
}

static void draw(JNIEnv *env,jobject thiz, jlong esContext, jint what){
    ESContext* _esContext = (ESContext*)esContext;
    if(_esContext == NULL){
        return;
    }

    if(_esContext->cb_what <= 0){
        _esContext->cb_what = what;
        findAndFillDrawMethods(_esContext, what);
        if( _esContext->initFunc != NULL ){
            _esContext->initFunc(_esContext);
        }
    }

    if( _esContext->cb_what != what ){
        if( _esContext->uninitFunc != NULL ){
            _esContext->uninitFunc(_esContext);
        }
        _esContext->cb_what = what;
        findAndFillDrawMethods(_esContext, what);

        if(_esContext->userData != NULL){
            free(_esContext->userData);
            _esContext->userData = NULL;
        }

        if( _esContext->initFunc != NULL ){
            _esContext->initFunc(_esContext);
        }
    }

    if (_esContext->drawFunc != NULL) {
        _esContext->drawFunc(_esContext);

        eglSwapBuffers(_esContext->eglDisplay, _esContext->eglSurface);
    }
}


#define GLAPI_CLASS_NAME "com/xucz/opengldemo/GLAPI"

static JNINativeMethod g_methods[] = {
        {"initGLEnvNative", "(Landroid/view/Surface;II)J", (void *)(initGLEnv)},
        {"uninitGLEnvNative", "(J)V", (void *)(uninitGLEnv)},
        {"drawNative", "(JI)V", (void *)(draw)}
};

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved){
    jint ret = JNI_ERR;
    JNIEnv  *env = NULL;
    if( (*vm)->GetEnv(vm, (void **) (&env), JNI_VERSION_1_6) != JNI_OK){
        return ret;
    }

    jclass clazz = (*env)->FindClass(env, GLAPI_CLASS_NAME);
    (*env)->RegisterNatives(env, clazz, g_methods, (int)(sizeof(g_methods) / sizeof(g_methods[0])));

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved){
    JNIEnv  *env = NULL;
    if( (*vm)->GetEnv(vm, (void **) (&env), JNI_VERSION_1_6) != JNI_OK){
        return;
    }
    jclass clazz = (*env)->FindClass(env, GLAPI_CLASS_NAME);
    (*env)->UnregisterNatives(env, clazz);
}