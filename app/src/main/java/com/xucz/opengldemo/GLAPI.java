package com.xucz.opengldemo;

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
public class GLAPI {

    // 图像数据格式，和ImageUtils.h的类型一一对应
    public static final int IMAGE_FORMAT_RGBA = 0x01;
    public static final int IMAGE_FORMAT_NV21 = 0x02;
    public static final int IMAGE_FORMAT_NV12 = 0x03;
    public static final int IMAGE_FORMAT_I420 = 0x04;

    static {
        System.loadLibrary("native-lib");
    }

    @Native
    private long mHandler = 0;

    public void initGLEnv(Surface surface) {
        mHandler = initGLEnvNative(surface);
    }

    /**
     * 创建离屏环境
     */
    public void initPBufferGLEnv(int width, int height) {
        mHandler = initPBufferGLEnvNative(width, height);
    }

    public void uninitGLEnv() {
        uninitGLEnvNative(mHandler);
        mHandler = 0;
    }

    public void draw(int what) {
        drawNative(mHandler, what);
    }

    public void setImageData(int format, int width, int height, byte[] byteArray) {
        setImageData(mHandler, format, width, height, byteArray);
    }

    public void changeTouchLoc(float x, float y) {
        changeTouchLocNative(mHandler, x, y);
    }

    public void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
        updateTransformMatrixNative(mHandler, rotateX, rotateY, scaleX, scaleY);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    private native long initGLEnvNative(Surface surface);

    private native long initPBufferGLEnvNative(int width, int height);

    private native void uninitGLEnvNative(long handler);

    private native void drawNative(long handler, int what);

    private native void setImageData(long handler, int format, int width, int height, byte[] byteArray);

    private native void changeTouchLocNative(long handler, float x, float y);

    private native void updateTransformMatrixNative(long handler, float rotateX, float rotateY, float scaleX, float scaleY);


}
