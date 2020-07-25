package com.xucz.opengldemo;


import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.view.Surface;
import android.view.SurfaceHolder;

public class GLRender implements Handler.Callback{
    private static final int FPS = 30;

    // 绘制类型
    public static final int WHAT_DRAW_BASE = 100;
    public static final int WHAT_DRAW_TRIANGLE = WHAT_DRAW_BASE + 1;
    public static final int WHAT_DRAW_VBO1= WHAT_DRAW_BASE + 2;
    public static final int WHAT_DRAW_VBO2 = WHAT_DRAW_BASE + 3;
    public static final int WHAT_DRAW_VAO = WHAT_DRAW_BASE + 4;

    // Handler消息
    private static final int WHAT_MSG_START_RENDER = 10;
    private static final int WHAT_MSG_STOP_RENDER = 11;
    private static final int WHAT_MSG_RENDER = 12;


    private final GLAPI glapi;
    private final HandlerThread mGlRenderThread;
    private final Handler mHandler;
    private volatile int mDrawWhat = 0;

    public GLRender(){
        glapi = new GLAPI();
        mGlRenderThread = new HandlerThread("GLRender");
        mGlRenderThread.start();
        mHandler = new Handler(mGlRenderThread.getLooper(), this);
    }


    public void startRender(Surface surface, int width, int height){
        mHandler.removeMessages(WHAT_MSG_START_RENDER);
        mHandler.removeMessages(WHAT_MSG_RENDER);
        Message msg = mHandler.obtainMessage(WHAT_MSG_START_RENDER, surface);
        msg.arg1 = width;
        msg.arg2 = height;
        mHandler.sendMessage(msg);
    }

    public void setDrawWhat(int mDrawWhat) {
        this.mDrawWhat = mDrawWhat;
    }

    public void stopRender(){
        mHandler.removeMessages(WHAT_MSG_STOP_RENDER);
        mHandler.removeMessages(WHAT_MSG_RENDER);
        mHandler.sendEmptyMessage(WHAT_MSG_STOP_RENDER);
    }

    @Override
    public boolean handleMessage(Message msg) {
        switch (msg.what) {
            case WHAT_MSG_START_RENDER:
                glapi.initGLEnv((Surface) msg.obj, msg.arg1, msg.arg2);

                mHandler.sendEmptyMessage(WHAT_MSG_RENDER);
                break;
            case WHAT_MSG_STOP_RENDER:
                glapi.uninitGLEnv();
                break;
            case WHAT_MSG_RENDER:
                long st = System.currentTimeMillis();
                glapi.draw(mDrawWhat);
                long ct = System.currentTimeMillis() - st;
                long delay = 1000 / FPS - ct;
                mHandler.sendEmptyMessageDelayed(WHAT_MSG_RENDER, delay);
                break;
        }
        return true;
    }

}
