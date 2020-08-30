package com.xucz.opengldemo.camera;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.view.Surface;

import com.xucz.opengldemo.MApplication;
import com.xucz.opengldemo.R;
import com.xucz.opengldemo.jni.GLCameraRender;
import com.xucz.opengldemo.utils.ImageUtils;


/**
 * Function:
 *
 * @author xucz
 * @since 2020/8/19
 */
public class CameraRender implements Handler.Callback{

    private static final int FPS = 30;

    private static final int MSG_WHAT_START_RENDER = 1;
    private static final int MSG_WHAT_STOP_RENDER = 2;
    private static final int MSG_WHAT_RENDER = 3;
    private static final int MSG_WHAT_DRAW_WHAT = 4;
    private static final int MSG_WHAT_CAMERA_TEXTURE_ID = 5;

    private long mShareContext;
    private GLCameraRender mGLCamera;

    private final HandlerThread cameraRenderThread;
    private final Handler mHandler;
    private int mDrawWhat = -1;
    private int mCameraTexId;
    private int mCameraTexType;
    private boolean hasGLInitialized = false;
    private int mCameraTexWidth;
    private int mCameraTexHeight;
    private int mCameraFacing;

    public CameraRender(long shareContext){
        mShareContext = shareContext;
        cameraRenderThread = new HandlerThread("CameraRender");
        cameraRenderThread.start();
        mHandler = new Handler(cameraRenderThread.getLooper(), this);

        mGLCamera = new GLCameraRender();
    }

    public void release(){
        mHandler.removeCallbacksAndMessages(null);
        stopRender();
        cameraRenderThread.quitSafely();
    }

    public void setCameraTexId(int texId, int texType, int width, int height, int facing){
        Object[] objs  =new Object[]{texId, texType, facing};
        mHandler.sendMessage(mHandler.obtainMessage(MSG_WHAT_CAMERA_TEXTURE_ID, width, height, objs));
    }

    public void startRender(Surface surface){
        mHandler.sendMessage(mHandler.obtainMessage(MSG_WHAT_START_RENDER, surface));
    }

    public void stopRender(){
        mHandler.sendEmptyMessage(MSG_WHAT_STOP_RENDER);
    }

    public void setDrawWhat(int what){
        mHandler.sendMessage(mHandler.obtainMessage(MSG_WHAT_DRAW_WHAT, what, 0));
    }

    @Override
    public boolean handleMessage(Message msg) {
        switch (msg.what) {
            case MSG_WHAT_START_RENDER:
                if(hasGLInitialized){
                    break;
                }
                mGLCamera.initGLEnv(mShareContext, (Surface) msg.obj);
                mGLCamera.setCameraTexId(mCameraTexId, mCameraTexType, mCameraTexWidth, mCameraTexHeight, mCameraFacing);
                hasGLInitialized = true;
                onDrawWhatChanged(mDrawWhat);
                mHandler.sendEmptyMessage(MSG_WHAT_RENDER);
                break;
            case MSG_WHAT_STOP_RENDER:
                mGLCamera.uninitGLEnv();
                hasGLInitialized = false;
                break;
            case MSG_WHAT_RENDER:
                long st = System.currentTimeMillis();
                if(mDrawWhat > 0){
                    mGLCamera.draw(mDrawWhat);
                }
                long ct = System.currentTimeMillis() - st;
                long delay = 1000 / FPS - ct;
                mHandler.sendEmptyMessageDelayed(MSG_WHAT_RENDER, delay);
                break;
            case MSG_WHAT_DRAW_WHAT:
                int _what = msg.arg1;
                onDrawWhatChanged(_what);
                mDrawWhat = _what;
                break;
            case MSG_WHAT_CAMERA_TEXTURE_ID:
                Object[] objs = (Object[]) msg.obj;
                mCameraTexId = (int) objs[0];
                mCameraTexType = (int) objs[1];
                mCameraFacing = (int) objs[2];
                mCameraTexWidth = msg.arg1;
                mCameraTexHeight = msg.arg2;
                if(hasGLInitialized){
                    mGLCamera.setCameraTexId(mCameraTexId, mCameraTexType, mCameraTexWidth, mCameraTexHeight, mCameraFacing);
                }
                break;
        }
        return true;
    }

    private void onDrawWhatChanged(int what) {
        switch (what){
            case GLCameraRender.WHAT_DRAW_CAMERA_LUT_FILTER_1:
                loadImage(R.drawable.lookup_glitch);
                break;
            case GLCameraRender.WHAT_DRAW_CAMERA_LUT_FILTER_2:
                loadImage(R.drawable.lookup_vertigo);
                break;
            case GLCameraRender.WHAT_DRAW_CAMERA_LUT_FILTER_3:
                loadImage(R.drawable.lut_a);
                break;
            case GLCameraRender.WHAT_DRAW_CAMERA_LUT_FILTER_4:
                loadImage(R.drawable.lut_b);
                break;
            case GLCameraRender.WHAT_DRAW_CAMERA_LUT_FILTER_5:
                loadImage(R.drawable.lut_c);
                break;
            case GLCameraRender.WHAT_DRAW_CAMERA_LUT_FILTER_6:
                loadImage(R.drawable.lut_d);
                break;
        }
    }

    private void loadImage(int resId) {
        ImageUtils.ImageData imageData = ImageUtils.loadRGBAImage(MApplication.getApplication(), resId, 0);
        if(imageData == null){
            return;
        }
        mGLCamera.setImageData(imageData.format, imageData.width, imageData.height, imageData.data);
    }

}
