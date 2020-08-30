//
// Created by xucz on 2020/8/23.
//

#include "gl_camera_capture_jni.h"
#include <jni.h>
#include <android/native_window_jni.h>



static jlong initPBufferGLEnv(JNIEnv *env,jobject thiz, jlong shareContext, jint width, jint height) {

    GLCameCapContext *glApiContext;
    glApiContext = malloc(sizeof(GLCameCapContext));
    memset(glApiContext, 0, sizeof(GLCameCapContext));

    ESContext *esContext = (ESContext *) glApiContext;
    esContext->eglNativeDisplay = EGL_DEFAULT_DISPLAY;

    ESContext* _shareContext = (ESContext*)shareContext;

    // 创建上下文
    esCreateWindow(esContext, _shareContext, ES_WINDOW_RGB | ES_WINDOW_PBUFFER, width, height);

    // 创建OES纹理
    GLuint textures[1];
    glGenTextures(1, textures);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, textures[0]);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, GL_NONE);
    glApiContext->oesTextureId = textures[0];

    return (jlong)glApiContext;
}

static void uninitGLEnv(JNIEnv *env,jobject thiz, jlong esContext){
    ESContext* _esContext = (ESContext*)esContext;
    if(_esContext == NULL){
        return;
    }

    GLCameCapContext *glApiContext = (GLCameCapContext *) _esContext;


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


static int getOESTextureId(JNIEnv *env,jobject thiz, jlong esContext){
    GLCameCapContext* _esContext = (GLCameCapContext*)esContext;
    if(_esContext == NULL){
        return 0;
    }
    return _esContext->oesTextureId;
}

static void releaseFBO(GLCameCapContext* context){
    if(!context){
        return;
    }
    if(context->fboId){
        glDeleteFramebuffers(1, &context->fboId);
        context->fboId = 0;
    }
    if(context->fboTextureId){
        glDeleteTextures(1, &context->fboTextureId);
        context->fboTextureId = 0;
    }
    if(context->program){
        glDeleteProgram(context->program);
        context->program = 0;
    }
    if(context->vboIds[0]){
        glDeleteBuffers(2, context->vboIds);
        context->vboIds[0] = 0;
    }
    if(context->vaoId){
        glDeleteVertexArrays(1, &context->vaoId);
        context->vaoId = 0;
    }
}

static void initFBO(GLCameCapContext* context){
    if(!context){
        return;
    }

    // 生成FBO
    glGenFramebuffers(1, &context->fboId);

    // 创建一个连接FBO的纹理
    glGenTextures(1, &context->fboTextureId);
    glBindTexture(GL_TEXTURE_2D, context->fboTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //   分配纹理内存
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, context->fboWidth, context->fboHeight, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 连接FBO和纹理
    glBindFramebuffer(GL_FRAMEBUFFER, context->fboId);
    glBindTexture(GL_TEXTURE_2D, context->fboTextureId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, context->fboTextureId, 0);
    //   检查连接状态
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        // error
        releaseFBO(context);
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 编译将OES纹理上的数据绘制到FBO上的程序
    char vFboShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec2 a_texCoord;   \n"
            "out vec2 v_texCoord;                       \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = a_position;               \n"
            "   v_texCoord = a_texCoord;                \n"
            "}                                          \n";

    char fFboShaderStr[] =
            "#version 300 es                            \n"
            "#extension GL_OES_EGL_image_external_essl3 : require\n"
            "precision mediump float;                   \n"
            "in vec2 v_texCoord;                        \n"
            "layout(location = 0) out vec4 outColor;    \n"
            "uniform samplerExternalOES s_TexSampler;   \n"
            "void main()                                \n"
            "{                                          \n"
            "    vec4 tempColor = texture(s_TexSampler, v_texCoord);\n"
            "    outColor = tempColor;\n"
            "}";

    GLuint program = esLoadProgram(vFboShaderStr, fFboShaderStr);
    if(!program){
        releaseFBO(context);
        return;
    }

    context->program = program;
    context->textureLoc = glGetUniformLocation(program, "s_TexSampler");

    // 确定顶点纹理坐标并缓存到VBO上
    GLfloat fbovertices[] = {
            -1.0f, 1.0f, 0, // v1
            0, 1.0f,  // t1
            -1.0f, -1.0f, 0, // v2
            0, 0, // t2
            1.0f, -1.0f, 0, // v3
            1.0f, 0, // t3
            1.0f, 1.0f, 0, //v4
            1.0f, 1.0f // t4
    };
    GLushort fboindices[] = {0, 1, 2, 0, 2, 3};

    glGenBuffers(2, context->vboIds);
    glBindBuffer(GL_ARRAY_BUFFER, context->vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbovertices), fbovertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fboindices), fboindices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

    // 使用VAO记录绘制过程
    glGenVertexArrays(1, &context->vaoId);
    glBindVertexArray(context->vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, context->vboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), ((void*)(3 * sizeof(GLfloat))));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->vboIds[1]);

    glBindVertexArray(GL_NONE);
}

static void mayDraw2Fbo(JNIEnv *env,jobject thiz, jlong esContext){
    GLCameCapContext* _esContext = (GLCameCapContext*)esContext;
    if(_esContext == NULL || _esContext->outTexType != TEX_TYPE_2D){
        return;
    }
    if(!_esContext->fboId || !_esContext->program){
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, _esContext->fboId);

    glViewport(0, 0, _esContext->fboWidth, _esContext->fboHeight);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.f, 1.f, 1.f, 0.f);

    glUseProgram(_esContext->program);
    glBindVertexArray(_esContext->vaoId);
    
    // 设置纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, _esContext->oesTextureId);
    glUniform1i(_esContext->textureLoc, 0);

    // 绘制
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, GL_NONE);
    glBindVertexArray(GL_NONE);


    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

static void setOutTexType(JNIEnv *env,jobject thiz, jlong esContext, jint texType, jint width, jint height){
    GLCameCapContext* _esContext = (GLCameCapContext*)esContext;
    if(_esContext == NULL){
        return;
    }
    _esContext->fboWidth = width;
    _esContext->fboHeight = height;
    _esContext->outTexType = texType;

    releaseFBO(_esContext);

    if(texType == TEX_TYPE_2D){
        initFBO(_esContext);
    }
}

static jint getOutTexType(JNIEnv *env,jobject thiz, jlong esContext){
    GLCameCapContext* _esContext = (GLCameCapContext*)esContext;
    if(_esContext == NULL){
        return 0;
    }
    return _esContext->outTexType;
}

static jint getOutTexId(JNIEnv *env,jobject thiz, jlong esContext){
    GLCameCapContext* _esContext = (GLCameCapContext*)esContext;
    if(_esContext == NULL){
        return 0;
    }
    int texType = _esContext->outTexType;
    if(texType == TEX_TYPE_2D){
        return _esContext->fboTextureId;
    }
    else{
        return _esContext->oesTextureId;
    }
}



#define GLAPI_CLASS_NAME "com/xucz/opengldemo/jni/GLCameraCapture"

static JNINativeMethod g_methods[] = {
        {"initPBufferGLEnvNative", "(JII)J", (void *)(initPBufferGLEnv)},
        {"uninitGLEnvNative", "(J)V", (void *)(uninitGLEnv)},
        {"getOESTextureIdNative", "(J)I", (void *)(getOESTextureId)},
        {"getOutTexIdNative", "(J)I", (void *)(getOutTexId)},
        {"getOutTexTypeNative", "(J)I", (void *)(getOutTexType)},
        {"setOutTexTypeNative", "(JIII)V", (void *)(setOutTexType)},
        {"mayDraw2FBO", "(J)V", (void *)(mayDraw2Fbo)},
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