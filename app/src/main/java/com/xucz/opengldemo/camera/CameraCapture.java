package com.xucz.opengldemo.camera;

import android.graphics.SurfaceTexture;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;

import com.xucz.opengldemo.jni.GLCameraCapture;

/**
 * Function:
 *
 *    相机采集线程
 *
 * @author xucz
 * @since 2020/8/16
 */
public class CameraCapture implements Handler.Callback {
    private static final String TAG = "CameraCapture";

    private static final int DEFAULT_PREVIEW_WIDTH = 1280;
    private static final int DEFAULT_PREVIEW_HEIGHT = 768;
    private static final int DEFAULT_FACING = GLCameraCapture.FACING_BACK;

    private static final int MSG_WHAT_INIT_GL_ENV      = 1;
    private static final int MSG_WHAT_UNINIT_GL_ENV    = 2;
    private static final int MSG_WHAT_START_CAPTURE    = 3;
    private static final int MSG_WHAT_STOP_CAPTUE = 4;
    private static final int MSG_WHAT_CHANGE_FACING = 5;
    private static final int MSG_WHAT_CHANGE_TEX_TYPE = 6;



    private HandlerThread mThread;
    private Handler mHandler;

    private GLCameraCapture mGLCamera;
    private CameraBase mCamera;
    private SurfaceTexture mSurfaceTexture;
    private volatile boolean isSurRelease = true;

    private Listener listener;

    private long mShareContext;

    private int mFacing = DEFAULT_FACING;
    private int mPreviewWidth = DEFAULT_PREVIEW_WIDTH;
    private int mPreviewHeight = DEFAULT_PREVIEW_HEIGHT;

    private boolean firstFrameRender = false;

    private int mTexType = GLCameraCapture.TEX_TYPE_OES;
    private boolean texTypeChange = false;

    private boolean isCapturing = false;

    public CameraCapture(long shareContext){
        mShareContext = shareContext;
        mThread = new HandlerThread("CameraCapture");
        mThread.start();
        mHandler = new Handler(mThread.getLooper(), this);
        mGLCamera = new GLCameraCapture();
        mCamera = CameraBase.newInstance();
        initGLEnv();
    }

    public void setListener(Listener listener) {
        this.listener = listener;
    }

    public void changeFacing(@GLCameraCapture.Facing int facing){
        mHandler.sendMessage(mHandler.obtainMessage(MSG_WHAT_CHANGE_FACING, facing, 0));
    }

    public void setTexType(@GLCameraCapture.OutTexType int texType) {
        mHandler.sendMessage(mHandler.obtainMessage(MSG_WHAT_CHANGE_TEX_TYPE, texType, 0));
    }

    public void release(){
        mHandler.removeCallbacksAndMessages(null);
        stopCapture();
        uninitGLEnv();
        mThread.quitSafely();
    }

    private void initGLEnv(){
        mHandler.sendEmptyMessage(MSG_WHAT_INIT_GL_ENV);
    }

    private void uninitGLEnv(){
        mHandler.sendEmptyMessage(MSG_WHAT_UNINIT_GL_ENV);
    }

    public void startCapture(){
        mHandler.sendEmptyMessage(MSG_WHAT_START_CAPTURE);
    }

    public void stopCapture(){
        mHandler.sendEmptyMessage(MSG_WHAT_STOP_CAPTUE);
    }

    @Override
    public boolean handleMessage(Message msg) {
        int what = msg.what;
        switch (what){
            case MSG_WHAT_INIT_GL_ENV:
                mGLCamera.initPBufferGLEnv(mShareContext, 1, 1);
                break;
            case MSG_WHAT_UNINIT_GL_ENV:
                mGLCamera.uninitGLEnv();
                break;
            case MSG_WHAT_START_CAPTURE:
                if(isCapturing){
                    break;
                }
                isCapturing = true;
                mCamera.setFacing(mFacing);

                mSurfaceTexture = new SurfaceTexture(mGLCamera.getOESTextureId());
                isSurRelease = false;
                mSurfaceTexture.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
                    @Override
                    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
                        if(!isSurRelease){
                            CameraCapture.this.onFrameAvailable(surfaceTexture);
                        }
                    }
                });
                mCamera.setSurfaceTexture(mSurfaceTexture);
                firstFrameRender = false;
                mCamera.open(mPreviewWidth, mPreviewHeight);

                break;
            case MSG_WHAT_CHANGE_FACING:
                mFacing = msg.arg1;
                firstFrameRender = false;
                mCamera.setFacing(mFacing);
                break;
            case MSG_WHAT_CHANGE_TEX_TYPE:
                mTexType = msg.arg1;
                texTypeChange = true;
                updateTexType();
                break;
            case MSG_WHAT_STOP_CAPTUE:
                isCapturing = false;
                mCamera.close();
                if(mSurfaceTexture != null){
                    isSurRelease = true;
                    mSurfaceTexture.release();
                    mSurfaceTexture = null;
                }
                break;
        }
        return false;
    }

    private void updateTexType() {
        if(firstFrameRender){
            mGLCamera.setOutTexType(mTexType,
                    mCamera.getPreviewSize().width,
                    mCamera.getPreviewSize().height);
        }
    }

    private void onFrameAvailable(SurfaceTexture surfaceTexture) {
        if(!firstFrameRender || texTypeChange){
            firstFrameRender = true;
            updateTexType();
            texTypeChange = false;
            if(listener != null){
                listener.onCaptureTexUpdate(mGLCamera.getOutTextureId(),
                        mTexType,
                        mCamera.getPreviewSize().width,
                        mCamera.getPreviewSize().height,
                        mFacing);
            }
        }
        surfaceTexture.updateTexImage();
        mGLCamera.mayDraw2FBO();
    }


    public interface Listener{
        void onCaptureTexUpdate(int textureId, int texType, int width, int height, int facing);
    }
}
