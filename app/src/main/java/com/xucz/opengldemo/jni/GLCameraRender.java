package com.xucz.opengldemo.jni;

import android.util.Log;
import android.view.Surface;

import com.xucz.opengldemo.camera.CameraBase;

import java.lang.annotation.Native;

/**
 * Function:
 *
 * @author xucz
 * @since 2020/8/23
 */
public class GLCameraRender {


    public static final int WHAT_DRAW_CAMERA                    = 0x00000100;
    public static final int WHAT_DRAW_CAMERA_NORMAL             = WHAT_DRAW_CAMERA + 1;
    public static final int WHAT_DRAW_CAMERA_GRID               = WHAT_DRAW_CAMERA + 2;
    public static final int WHAT_DRAW_CAMERA_SPLIT_SCREEN       = WHAT_DRAW_CAMERA + 3;
    public static final int WHAT_DRAW_CAMERA_SCALE_CIRCLE       = WHAT_DRAW_CAMERA + 4;
    public static final int WHAT_DRAW_CAMERA_LUT_FILTER_1       = WHAT_DRAW_CAMERA + 5;
    public static final int WHAT_DRAW_CAMERA_LUT_FILTER_2       = WHAT_DRAW_CAMERA + 6;
    public static final int WHAT_DRAW_CAMERA_LUT_FILTER_3       = WHAT_DRAW_CAMERA + 7;
    public static final int WHAT_DRAW_CAMERA_LUT_FILTER_4       = WHAT_DRAW_CAMERA + 8;
    public static final int WHAT_DRAW_CAMERA_LUT_FILTER_5       = WHAT_DRAW_CAMERA + 9;
    public static final int WHAT_DRAW_CAMERA_LUT_FILTER_6       = WHAT_DRAW_CAMERA + 10;
    public static final int WHAT_DRAW_CAMERA_SEPARATION_SHIFT   = WHAT_DRAW_CAMERA + 11;
    public static final int WHAT_DRAW_CAMERA_SOUL_OUT           = WHAT_DRAW_CAMERA + 12;
    public static final int WHAT_DRAW_CAMERA_ROTATE_CIRCLE      = WHAT_DRAW_CAMERA + 13;
    public static final int WHAT_DRAW_CAMERA_PICT_IN_PICT       = WHAT_DRAW_CAMERA + 14;

    static {
        try {
            System.loadLibrary("gl_camera_render");
        } catch (Exception e) {
            Log.e("CameraContext", "", e);
        }
    }

    @Native
    private long mHandler = 0;


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

    /**
     * 设置相机纹理
     */
    public void setCameraTexId(int texId, int texType, int width, int height, @GLCameraCapture.Facing int facing) {
        setCameraTexIdNative(mHandler, texId, texType, width, height, facing);
    }

    public void draw(int what) {
        drawNative(mHandler, what);
    }

    public long getContext(){
        return mHandler;
    }


    /**
     * 加载LUTl图片等
     */
    public void setImageData(int format, int width, int height, byte[] data){
        setImageDataNative(mHandler, 0, format, width, height, data);
    }

    private native long initGLEnvNative(long shareContext, Surface surface);

    private native long initPBufferGLEnvNative(long shareContext, int width, int height);

    private native void uninitGLEnvNative(long handler);

    private native void drawNative(long handler, int what);

    private native void setCameraTexIdNative(long handler, int texId, int texType, int width, int height, int facing);

    private native void setImageDataNative(long handler, int index, int format, int width, int height, byte[] byteArray);

}
