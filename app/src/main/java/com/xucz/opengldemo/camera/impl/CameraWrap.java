package com.xucz.opengldemo.camera.impl;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.util.Log;

import com.xucz.opengldemo.camera.CameraBase;
import com.xucz.opengldemo.jni.GLCameraCapture;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import static com.xucz.opengldemo.jni.GLCameraCapture.FACING_BACK;
import static com.xucz.opengldemo.jni.GLCameraCapture.FACING_FRONT;

/**
 * Function:
 *
 * @author xucz
 * @since 2020/8/20
 */
public class CameraWrap extends CameraBase {

    private Camera mCamera;
    private int mFacing = FACING_FRONT;
    private int mReqWidth, mReqHeight;
    private int mRealWidth, mRealHeight;
    private SurfaceTexture surfaceTexture;


    @Override
    public void open(int width, int height) {
        mReqWidth = width;
        mReqHeight = height;
        initCamera();

    }

    @Override
    public void setFacing(@GLCameraCapture.Facing int facing) {
        if (mFacing == facing) {
            return;
        }
        mFacing = facing;
        if (mCamera != null) {
            uninitCamera();
            initCamera();
        }
    }

    @Override
    public void close() {
        uninitCamera();
    }

    @Override
    public void setSurfaceTexture(SurfaceTexture surfaceTexture) {
        this.surfaceTexture = surfaceTexture;
        if (mCamera != null && surfaceTexture != null) {
            try {
                mCamera.setPreviewTexture(surfaceTexture);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }


    @Override
    public List<Size> getSupportSize() {
        List<Size> sizes = new ArrayList<>();
        if (mCamera != null) {
            List<Camera.Size> supportedPreviewSizes = mCamera.getParameters().getSupportedPreviewSizes();
            for (Camera.Size size : supportedPreviewSizes) {
                Size e = new Size();
                e.width = size.width;
                e.height = size.height;
                sizes.add(e);
            }
        }
        return null;
    }


    private void initCamera() {
        if (mCamera != null) {
            return;
        }
        int cameraFacing = 0;
        if (mFacing == FACING_FRONT) {
            cameraFacing = Camera.CameraInfo.CAMERA_FACING_FRONT;
        } else if (mFacing == FACING_BACK) {
            cameraFacing = Camera.CameraInfo.CAMERA_FACING_BACK;
        }

        mCamera = Camera.open(cameraFacing);
        Camera.Parameters parameters = mCamera.getParameters();
        findProperSize(parameters);
        parameters.setPreviewSize(mRealWidth, mRealHeight);
        mCamera.setParameters(parameters);

        if(previewListener != null){
            previewListener.onPreviewSizeChanged(mFacing, mRealWidth, mRealHeight);
        }

        setSurfaceTexture(surfaceTexture);
        mCamera.startPreview();
    }

    private void findProperSize(Camera.Parameters parameters) {
        List<Camera.Size> supportedPreviewSizes = parameters.getSupportedPreviewSizes();
        // 查找是否有同尺寸
        float ration = 1.0f * mReqWidth / mReqHeight;
        int widthDiff = Integer.MAX_VALUE;
        float rDiff = Float.MAX_VALUE;
        for (Camera.Size supportedPreviewSize : supportedPreviewSizes) {
            float r = 1.0f * supportedPreviewSize.width / supportedPreviewSize.height;
            float rd = Math.abs(r - ration);
            int wd = Math.abs(supportedPreviewSize.width - mReqWidth);
            if(rd <= rDiff && wd <= widthDiff){
                rDiff = rd;
                widthDiff = wd;
                mRealWidth = supportedPreviewSize.width;
                mRealHeight = supportedPreviewSize.height;
            }
        }
        Log.d(TAG, String.format(Locale.US, "ReqWidth:%d, ReqHeight:%d, RealWidth:%d, RealHeight:%d",
                mReqWidth, mReqHeight, mRealWidth, mRealHeight));
    }

    @Override
    public Size getPreviewSize() {
        return new Size(mRealWidth, mRealHeight);
    }

    private void uninitCamera() {
        if (mCamera != null) {
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }
    }


}
