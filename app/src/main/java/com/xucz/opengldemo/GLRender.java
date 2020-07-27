package com.xucz.opengldemo;


import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.view.Surface;

import java.io.IOException;
import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;

import static com.xucz.opengldemo.GLAPI.IMAGE_FORMAT_NV21;
import static com.xucz.opengldemo.GLAPI.IMAGE_FORMAT_RGBA;

public class GLRender implements Handler.Callback{
    private static final int FPS = 30;

    // 绘制类型，和GLAPI_native.h中的类型一一对应
    public static final int WHAT_DRAW_BASE              = 100;
    public static final int WHAT_DRAW_TRIANGLE          = WHAT_DRAW_BASE + 1;
    public static final int WHAT_DRAW_TEXTUREMAP        = WHAT_DRAW_BASE + 2;
    public static final int WHAT_DRAW_YUVTEXTUREMAP     = WHAT_DRAW_BASE + 3;
    public static final int WHAT_DRAW_VBO              = WHAT_DRAW_BASE + 4;
    public static final int WHAT_DRAW_VAO               = WHAT_DRAW_BASE + 5;
    public static final int WHAT_DRAW_FBO               = WHAT_DRAW_BASE + 6;

    // Handler消息
    private static final int WHAT_MSG_START_RENDER = 10;
    private static final int WHAT_MSG_STOP_RENDER = 11;
    private static final int WHAT_MSG_RENDER = 12;
    private static final int WHAT_MSG_DRAW_WHAT = 13;


    private final GLAPI glapi;
    private final HandlerThread mGlRenderThread;
    private final Handler mHandler;
    private volatile int mDrawWhat = 0;

    private WeakReference<Context> contextWeakReference;

    public GLRender(Context context){
        contextWeakReference = new WeakReference<>(context);
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
        mHandler.sendMessage(mHandler.obtainMessage(WHAT_MSG_DRAW_WHAT, mDrawWhat, 0));
    }

    public void stopRender(){
        mHandler.removeMessages(WHAT_MSG_STOP_RENDER);
        mHandler.removeMessages(WHAT_MSG_RENDER);
        mHandler.sendEmptyMessage(WHAT_MSG_STOP_RENDER);
    }

    public void release(){
        stopRender();
        mGlRenderThread.quit();
    }

    @Override
    public boolean handleMessage(Message msg) {
        switch (msg.what) {
            case WHAT_MSG_DRAW_WHAT:
                mDrawWhat = msg.arg1;
                onDrawWhatChanged();
                break;
            case WHAT_MSG_START_RENDER:
                glapi.initGLEnv((Surface) msg.obj, msg.arg1, msg.arg2);
                onDrawWhatChanged();
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

    private void onDrawWhatChanged() {
        switch (mDrawWhat){
            case WHAT_DRAW_TEXTUREMAP:
                loadRGBAImage(R.drawable.dzzz);
                break;
            case WHAT_DRAW_YUVTEXTUREMAP:
                loadNV21Image();
                break;
            case WHAT_DRAW_FBO:
                loadRGBAImage(R.drawable.java);
                break;
        }
    }

    private void loadNV21Image() {
        if (contextWeakReference == null || contextWeakReference.get() == null) {
            return;
        }
        Context context = contextWeakReference.get();
        InputStream is = null;
        try {
            is = context.getAssets().open("YUV_Image_840x1074.NV21");
        } catch (IOException e) {
            e.printStackTrace();
        }

        int lenght = 0;
        try {
            lenght = is.available();
            byte[] buffer = new byte[lenght];
            is.read(buffer);
            glapi.setImageData(IMAGE_FORMAT_NV21, 840, 1074, buffer);
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try
            {
                is.close();
            }
            catch(IOException e)
            {
                e.printStackTrace();
            }
        }

    }

    private void loadRGBAImage(int resId) {
        if (contextWeakReference == null || contextWeakReference.get() == null) {
            return;
        }
        Context context = contextWeakReference.get();
        InputStream is = context.getResources().openRawResource(resId);
        Bitmap bitmap;
        try {
            bitmap = BitmapFactory.decodeStream(is);
            if (bitmap != null) {
                int bytes = bitmap.getByteCount();
                ByteBuffer buf = ByteBuffer.allocate(bytes);
                bitmap.copyPixelsToBuffer(buf);
                byte[] byteArray = buf.array();
                glapi.setImageData(IMAGE_FORMAT_RGBA, bitmap.getWidth(), bitmap.getHeight(), byteArray);
            }
        }
        finally
        {
            try
            {
                is.close();
            }
            catch(IOException e)
            {
                e.printStackTrace();
            }
        }
    }

}
