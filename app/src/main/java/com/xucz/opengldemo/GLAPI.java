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

    static {
        System.loadLibrary("native-lib");
    }

    @Native
    private long mHandler = 0;

    public void initGLEnv(Surface surface, int width, int height){
        mHandler = initGLEnvNative(surface, width, height);
    }

    public void uninitGLEnv(){
        uninitGLEnvNative(mHandler);
        mHandler = 0;
    }

    public void draw(int what){
        drawNative(mHandler, what);
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    private native long initGLEnvNative(Surface surface, int width, int height);

    private native void uninitGLEnvNative(long handler);

    private native void drawNative(long handler, int what);

}
