package com.xucz.opengldemo.jni;

import android.support.annotation.IntDef;
import android.util.Log;
import android.view.Surface;

import java.lang.annotation.Native;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * Function:
 *
 * @author xucz
 * @since 2020/8/23
 */
public class GLCameraCapture {

    static {
        try {
            System.loadLibrary("gl_camera_capture");
        } catch (Exception e) {
            Log.e("GLCameraCapture", "", e);
        }
    }

    public static final int TEX_TYPE_OES = 1;
    public static final int TEX_TYPE_2D  = 2;

    @IntDef({TEX_TYPE_OES, TEX_TYPE_2D})
    @Retention(RetentionPolicy.RUNTIME)
    public @interface OutTexType{}

    // 后置摄像头
    public static final int FACING_BACK = 1;
    // 前置摄像头
    public static final int FACING_FRONT = 2;

    @IntDef({FACING_BACK, FACING_FRONT})
    @Retention(RetentionPolicy.SOURCE)
    public @interface Facing{}

    @Native
    private long mHandler = 0;

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


    public void setOutTexType(@OutTexType int texType, int width, int height){
        setOutTexTypeNative(mHandler, texType, width, height);
    }

    public int getOESTextureId(){
        return getOESTextureIdNative(mHandler);
    }

    public int getOutTextureId(){
        return getOutTexIdNative(mHandler);
    }

    public int getOutTextureType(){
        return getOutTexTypeNative(mHandler);
    }

    public void mayDraw2FBO(){
        mayDraw2FBO(mHandler);
    }


    private native long initPBufferGLEnvNative(long shareContext, int width, int height);

    private native void uninitGLEnvNative(long handler);

    private native void setOutTexTypeNative(long handler, int texType, int width, int height);

    private native int getOESTextureIdNative(long handler);

    private native int getOutTexIdNative(long handler);

    private native int getOutTexTypeNative(long handler);

    private native void mayDraw2FBO(long handler);

}
