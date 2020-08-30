//
// Created by xucz on 2020/8/23.
//

#include "gl_camera_render_jni.h"
#include <jni.h>
#include <android/native_window_jni.h>


static jlong initGLEnv(JNIEnv *env,jobject thiz, jlong shareContext, jobject android_surface) {

    ANativeWindow *native_window = NULL;
    if (android_surface) {
        native_window = ANativeWindow_fromSurface(env, android_surface);
        if (!native_window) {
            ALOGE("%s: ANativeWindow_fromSurface: failed\n", __func__);
            return 0;
        }
    }

    GLCameraContext *glApiContext;
    glApiContext = malloc(sizeof(GLCameraContext));
    memset(glApiContext, 0, sizeof(GLCameraContext));

    ESContext *esContext = (ESContext *) glApiContext;

    esContext->eglNativeDisplay = EGL_DEFAULT_DISPLAY;
    esContext->eglNativeWindow = native_window;

    ESContext* _shareContext = (ESContext*)shareContext;

    // 使用深度测试时记得要先开启
    esCreateWindow(esContext, _shareContext, ES_WINDOW_RGB, 0, 0);

    return (jlong)glApiContext;
}

static jlong initPBufferGLEnv(JNIEnv *env,jobject thiz, jlong shareContext, jint width, jint height) {

    GLCameraContext *glApiContext;
    glApiContext = malloc(sizeof(GLCameraContext));
    memset(glApiContext, 0, sizeof(GLCameraContext));

    ESContext *esContext = (ESContext *) glApiContext;
    esContext->eglNativeDisplay = EGL_DEFAULT_DISPLAY;

    ESContext* _shareContext = (ESContext*)shareContext;

    esCreateWindow(esContext, _shareContext, ES_WINDOW_RGB | ES_WINDOW_PBUFFER, width, height);

    return (jlong)glApiContext;
}

static void uninitGLEnv(JNIEnv *env,jobject thiz, jlong esContext){
    ESContext* _esContext = (ESContext*)esContext;
    if(_esContext == NULL){
        return;
    }

    GLCameraContext *glApiContext = (GLCameraContext *) _esContext;

    if (glApiContext->uninitFunc != NULL) {
        glApiContext->uninitFunc(_esContext);
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

    GLCameraContext *pGlCameraContext = (GLCameraContext *) _esContext;

    if(!pGlCameraContext->cameraTexId){
        return;
    }

    if(pGlCameraContext->cb_what <= 0){
        pGlCameraContext->cb_what = what;
        findAndFillDrawCameraMethods(pGlCameraContext, what);
        if(pGlCameraContext->initFunc != NULL ){
            pGlCameraContext->initFunc(_esContext);
        }

        if(pGlCameraContext->setCameraTexId){
            pGlCameraContext->setCameraTexId(_esContext, pGlCameraContext->cameraTexId,
                                             pGlCameraContext->cameraTexType,
                                             pGlCameraContext->cameraTexWidth,
                                             pGlCameraContext->cameraTexHeight,
                                             pGlCameraContext->cameraFacing);
        }
    }

    if(pGlCameraContext->cb_what != what ){
        if(pGlCameraContext->uninitFunc != NULL ){
            pGlCameraContext->uninitFunc(_esContext);
        }
        pGlCameraContext->cb_what = what;
        findAndFillDrawCameraMethods(pGlCameraContext, what);

        if(_esContext->userData != NULL){
            free(_esContext->userData);
            _esContext->userData = NULL;
        }

        if(pGlCameraContext->initFunc != NULL ){
            pGlCameraContext->initFunc(_esContext);
        }

        if(pGlCameraContext->setCameraTexId){
            pGlCameraContext->setCameraTexId(_esContext, pGlCameraContext->cameraTexId,
                                             pGlCameraContext->cameraTexType,
                                             pGlCameraContext->cameraTexWidth,
                                             pGlCameraContext->cameraTexHeight,
                                             pGlCameraContext->cameraFacing);
        }
    }

    if (pGlCameraContext->drawFunc != NULL) {
        pGlCameraContext->drawFunc(_esContext);

        eglSwapBuffers(_esContext->eglDisplay, _esContext->eglSurface);
    }
}



static void setCameraTexId(JNIEnv *env,jobject thiz, jlong esContext,
        jint textureId, jint texType, jint widht, jint height, jint facing){
    ESContext* _esContext = (ESContext*)esContext;
    if(_esContext == NULL){
        return;
    }
    GLCameraContext *glCameraContext = (GLCameraContext *) _esContext;

    glCameraContext->cameraTexId = textureId;
    glCameraContext->cameraTexType = texType;
    glCameraContext->cameraTexWidth = widht;
    glCameraContext->cameraTexHeight = height;
    glCameraContext->cameraFacing = facing;

    if(glCameraContext->setCameraTexId){
        glCameraContext->setCameraTexId(_esContext, textureId, texType, widht, height, facing);
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

#define GLAPI_CLASS_NAME "com/xucz/opengldemo/jni/GLCameraRender"

static JNINativeMethod g_methods[] = {
        {"initGLEnvNative", "(JLandroid/view/Surface;)J", (void *)(initGLEnv)},
        {"initPBufferGLEnvNative", "(JII)J", (void *)(initPBufferGLEnv)},
        {"uninitGLEnvNative", "(J)V", (void *)(uninitGLEnv)},
        {"drawNative", "(JI)V", (void *)(draw)},
        {"setCameraTexIdNative", "(JIIIII)V", (void *)(setCameraTexId)},
        {"setImageDataNative", "(JIIII[B)V", (void *)(setImageData)},
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