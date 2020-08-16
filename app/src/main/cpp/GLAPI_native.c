#include <jni.h>
#include <android/native_window_jni.h>
#include "GLAPI_native.h"

static jlong initGLEnv(JNIEnv *env,jobject thiz, jobject android_surface) {

    ANativeWindow *native_window = NULL;
    if (android_surface) {
        native_window = ANativeWindow_fromSurface(env, android_surface);
        if (!native_window) {
            ALOGE("%s: ANativeWindow_fromSurface: failed\n", __func__);
            return 0;
        }
    }

    ESContext *esContext;
    esContext = malloc(sizeof(ESContext));
    memset(esContext, 0, sizeof(ESContext));

    esContext->eglNativeDisplay = EGL_DEFAULT_DISPLAY;
    esContext->eglNativeWindow = native_window;

    // 使用深度测试时记得要先开启
    esCreateWindow(esContext, ES_WINDOW_RGB | ES_WINDOW_DEPTH | ES_WINDOW_STENCIL | ES_WINDOW_MULTISAMPLE, 0, 0);

    return (jlong)esContext;
}

static jlong initPBufferGLEnv(JNIEnv *env,jobject thiz, jint width, jint height) {

    ESContext *esContext;
    esContext = malloc(sizeof(ESContext));
    memset(esContext, 0, sizeof(ESContext));

    esContext->eglNativeDisplay = EGL_DEFAULT_DISPLAY;

    esCreateWindow(esContext, ES_WINDOW_RGB | ES_WINDOW_PBUFFER, width, height);

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

    size_t length = sizeof(_esContext->imageData) / sizeof(void *);
    LOGCATE("uninitGLEnv imageData length : %d", length);
    for(int i=0; i < length; i++){
        if( _esContext->imageData[i] != NULL){
            freeNativeImage((NativeImage* )_esContext->imageData[i]);
            free(_esContext->imageData[i]);
            _esContext->imageData[i] = NULL;
        }
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

static void setImageData(JNIEnv *env,jobject thiz, jlong esContext, jint index, jint format, jint width, jint height, jbyteArray imageData){
    ESContext* _esContext = (ESContext*)esContext;
    if(_esContext == NULL){
        return;
    }
    jsize length = (*env)->GetArrayLength(env, imageData);
    uint8_t *buf = malloc(length);
    (*env)->GetByteArrayRegion(env, imageData, 0, length, buf);

    if( _esContext->imageData[index] != NULL ){
        freeNativeImage(_esContext->imageData[index]);
        free(_esContext->imageData[index]);
    }

    NativeImage *image = NULL;
    allocNativeImage2(&image, format, width, height, buf);

    _esContext->imageData[index] = image;

    free(buf);
    (*env)->DeleteLocalRef(env, imageData);
}

static void changeTouchLoc(JNIEnv *env,jobject thiz, jlong esContext, jfloat x, jfloat y){
    ESContext* _esContext = (ESContext*)esContext;
    if(_esContext == NULL){
        return;
    }
    if(_esContext->updateTouchLoc){
        _esContext->updateTouchLoc(_esContext, x, y);
    }
}

static void updateTransformMatrix(JNIEnv *env,jobject thiz, jlong esContext,
        jfloat rotateX, jfloat rotateY, jfloat scaleX, jfloat scaleY){
    ESContext* _esContext = (ESContext*)esContext;
    if(_esContext == NULL){
        return;
    }
    if(_esContext->updateTransformMatrix){
        _esContext->updateTransformMatrix(_esContext, rotateX, rotateY, scaleX, scaleY);
    }
}


#define GLAPI_CLASS_NAME "com/xucz/opengldemo/GLAPI"

static JNINativeMethod g_methods[] = {
        {"initGLEnvNative", "(Landroid/view/Surface;)J", (void *)(initGLEnv)},
        {"initPBufferGLEnvNative", "(II)J", (void *)(initPBufferGLEnv)},
        {"uninitGLEnvNative", "(J)V", (void *)(uninitGLEnv)},
        {"drawNative", "(JI)V", (void *)(draw)},
        {"setImageData", "(JIIII[B)V", (void *)(setImageData)},
        {"changeTouchLocNative", "(JFF)V", (void *)(changeTouchLoc)},
        {"updateTransformMatrixNative", "(JFFFF)V", (void *)(updateTransformMatrix)}
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