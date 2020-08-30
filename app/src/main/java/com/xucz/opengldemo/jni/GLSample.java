package com.xucz.opengldemo.jni;

import android.util.Log;
import android.view.Surface;

import java.lang.annotation.Native;

/**
 * 描述：
 *
 * @author 创建人 ：xucz
 * @version 1.0
 * @createTime 创建时间 ：2019-06-22
 * @modifyBy 修改人 ：
 * @modifyTime 修改时间 ：
 * @modifyMemo 修改备注：
 */
public class GLSample {

    // 绘制类型，和GLAPI_native.h中的类型一一对应
    public static final int WHAT_DRAW_BASE              = 0x00000010;
    public static final int WHAT_DRAW_TRIANGLE          = WHAT_DRAW_BASE + 1;
    public static final int WHAT_DRAW_TEXTUREMAP        = WHAT_DRAW_BASE + 2;
    public static final int WHAT_DRAW_YUVTEXTUREMAP     = WHAT_DRAW_BASE + 3;
    public static final int WHAT_DRAW_VBO               = WHAT_DRAW_BASE + 4;
    public static final int WHAT_DRAW_VAO               = WHAT_DRAW_BASE + 5;
    public static final int WHAT_DRAW_FBO               = WHAT_DRAW_BASE + 6;
    public static final int WHAT_DRAW_TRANSFORM_FEEDBACK= WHAT_DRAW_BASE + 7;
    public static final int WHAT_DRAW_COORDINATE_SYSTEM = WHAT_DRAW_BASE + 8;
    public static final int WHAT_DRAW_BASIC_LIGHTING    = WHAT_DRAW_BASE + 9;
    public static final int WHAT_DRAW_DEPTH_TESTING     = WHAT_DRAW_BASE + 10;
    public static final int WHAT_DRAW_STENCIL_TESTING   = WHAT_DRAW_BASE + 11;
    public static final int WHAT_DRAW_BLENDING          = WHAT_DRAW_BASE + 12;
    public static final int WHAT_DRAW_INSTANCING        = WHAT_DRAW_BASE + 13;
    public static final int WHAT_DRAW_INSTANCING3D      = WHAT_DRAW_BASE + 14;
    public static final int WHAT_DRAW_PARTICLES         = WHAT_DRAW_BASE + 15;
    public static final int WHAT_DRAW_SKYBOX            = WHAT_DRAW_BASE + 16;

    // EGL后台绘制类型，对应GLAPI_native.h中的类型
    public static final int WHAT_DRAW_EGL = 0x00001000;
    public static final int WHAT_DRAW_EGL_NORMAL      = WHAT_DRAW_EGL + 1;
    public static final int WHAT_DRAW_EGL_MOSAIC      = WHAT_DRAW_EGL + 2;
    public static final int WHAT_DRAW_EGL_GRID        = WHAT_DRAW_EGL + 3;
    public static final int WHAT_DRAW_EGL_ROTATE      = WHAT_DRAW_EGL + 4;
    public static final int WHAT_DRAW_EGL_EDGE        = WHAT_DRAW_EGL + 5;
    public static final int WHAT_DRAW_EGL_ENLARGE     = WHAT_DRAW_EGL + 6;
    public static final int WHAT_DRAW_EGL_UNKNOW      = WHAT_DRAW_EGL + 7;
    public static final int WHAT_DRAW_EGL_DEFORMATION = WHAT_DRAW_EGL + 8;

    static {
        try {
            System.loadLibrary("gl_sample");
        } catch (Exception e) {
            Log.e("GLSample", "", e);
        }
    }

    @Native
    private long mHandler = 0;

    public void initGLEnv(Surface surface) {
        initGLEnv(0, surface);
    }

    public void initGLEnv(long shareContext, Surface surface) {
        mHandler = initGLEnvNative(shareContext, surface);
    }

    public void initPBufferGLEnv(int width, int height) {
        initPBufferGLEnv(0, width, height);
    }

    /**
     * 创建离屏环境
     */
    public void initPBufferGLEnv(long shareContext, int width, int height) {
        mHandler = initPBufferGLEnvNative(shareContext, width, height);
    }

    public void uninitGLEnv() {
        uninitGLEnvNative(mHandler);
        mHandler = 0;
    }


    public void draw(int what) {
        drawNative(mHandler, what);
    }

    public void setImageData(int format, int width, int height, byte[] byteArray) {
        setImageData(mHandler, 0, format, width, height, byteArray);
    }

    public void setImageData(int index, int format, int width, int height, byte[] byteArray) {
        setImageData(mHandler, index, format, width, height, byteArray);
    }

    public void changeTouchLoc(float x, float y) {
        changeTouchLocNative(mHandler, x, y);
    }

    public void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
        updateTransformMatrixNative(mHandler, rotateX, rotateY, scaleX, scaleY);
    }

    public long getContext(){
        return mHandler;
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    private native long initGLEnvNative(long shareContext, Surface surface);

    private native long initPBufferGLEnvNative(long shareContext, int width, int height);

    private native void uninitGLEnvNative(long handler);

    private native void drawNative(long handler, int what);

    private native void setImageData(long handler, int index, int format, int width, int height, byte[] byteArray);

    private native void changeTouchLocNative(long handler, float x, float y);

    private native void updateTransformMatrixNative(long handler, float rotateX, float rotateY, float scaleX, float scaleY);

}
