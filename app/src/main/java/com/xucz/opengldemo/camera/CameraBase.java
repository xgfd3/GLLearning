package com.xucz.opengldemo.camera;

import android.graphics.SurfaceTexture;
import android.support.annotation.IntDef;

import com.xucz.opengldemo.camera.impl.CameraWrap;
import com.xucz.opengldemo.jni.GLCameraCapture;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.List;

/**
 * Function:
 *
 * @author xucz
 * @since 2020/8/20
 */
public abstract class CameraBase {

    protected final String TAG = this.getClass().getSimpleName();

    public static CameraBase newInstance(){
        return new CameraWrap();
    }



    protected PreviewListener previewListener;

    public abstract void open(int width, int height);

    public abstract void setFacing(@GLCameraCapture.Facing int facing);

    public abstract void close();

    public abstract void setSurfaceTexture(SurfaceTexture surfaceTexture);

    public void setPreviewListener(PreviewListener previewListener){
        this.previewListener = previewListener;
    }

    public abstract List<Size> getSupportSize();

    public abstract Size getPreviewSize();

    public interface PreviewListener{
        void onPreviewSizeChanged(int facing, int width, int height);
    }

    public static class Size{
        public int width;
        public int height;

        public Size(){}

        public Size(int width, int height){
            this.width = width;
            this.height = height;
        }

        @Override
        public String toString() {
            return width + "x" + height;
        }
    }

}
