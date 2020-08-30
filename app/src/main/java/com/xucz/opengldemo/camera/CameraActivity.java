package com.xucz.opengldemo.camera;

import android.Manifest;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SubMenu;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.Toast;

import com.xucz.opengldemo.R;
import com.xucz.opengldemo.jni.GLCameraCapture;
import com.xucz.opengldemo.jni.GLCameraRender;

import static android.content.pm.PackageManager.PERMISSION_GRANTED;
import static com.xucz.opengldemo.jni.GLCameraRender.*;

/**
 * Function: 相机
 *
 * @author xucz
 * @since 2020/8/16
 */
public class CameraActivity extends AppCompatActivity {
    private static final int PERMISSION_REQUEST_CODE = 100;


    private static final int CAMERA_ITEM_GROUP_ID = 0x00000000;
    private static final int CAMERA_ITEM_ID_FRONT = CAMERA_ITEM_GROUP_ID + 1;
    private static final int CAMERA_ITEM_ID_BACK = CAMERA_ITEM_GROUP_ID + 2;
    private static final int CAMERA_ITEM_ID_OUT_OES = CAMERA_ITEM_GROUP_ID + 3;
    private static final int CAMERA_ITEM_ID_OUT_FBO = CAMERA_ITEM_GROUP_ID + 4;


    private GLCameraRender mRootGL;

    private CameraCapture cameraCapture;
    private CameraRender cameraRender;

    private SurfaceView surfaceView;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);
        surfaceView = findViewById(R.id.surfaceview);

        checkPermission();
    }

    private void checkPermission() {
        String[] permissions = new String[]{Manifest.permission.CAMERA};
        boolean hasPermission = true;
        for (String permission : permissions) {
            int i = ContextCompat.checkSelfPermission(this, permission);
            if (i != PERMISSION_GRANTED) {
                hasPermission = false;
                break;
            }
        }
        if (hasPermission) {
            init();
        } else {
            ActivityCompat.requestPermissions(this, permissions, PERMISSION_REQUEST_CODE);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        boolean hasGranted = true;
        for (int grantResult : grantResults) {
            if (grantResult != PERMISSION_GRANTED) {
                hasGranted = false;
                break;
            }
        }
        if (hasGranted) {
            init();
        } else {
            Toast.makeText(this, "相机权限被禁用", Toast.LENGTH_LONG).show();
        }
    }

    private void init() {
        // 初始化全局GL环境，用于共享上下文
        mRootGL = new GLCameraRender();
        mRootGL.initPBufferGLEnv(1, 1);

        // 视频采集
        cameraCapture = new CameraCapture(mRootGL.getContext());
        cameraCapture.setListener(new CameraCapture.Listener() {
            @Override
            public void onCaptureTexUpdate(int textureId, int texType, int width, int height, int facing) {
                if (cameraRender != null) {
                    cameraRender.setCameraTexId(textureId, texType, width, height, facing);
                }
            }
        });
        cameraCapture.startCapture();

        // 视频渲染
        cameraRender = new CameraRender(mRootGL.getContext());
        cameraRender.setDrawWhat(WHAT_DRAW_CAMERA_NORMAL);
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                cameraRender.startRender(holder.getSurface());
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                cameraRender.stopRender();
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        SubMenu cameraOpt = menu.addSubMenu(CAMERA_ITEM_GROUP_ID, 0, 0, "相机");
        cameraOpt.add(CAMERA_ITEM_GROUP_ID, CAMERA_ITEM_ID_FRONT, 0, "前置摄像头");
        cameraOpt.add(CAMERA_ITEM_GROUP_ID, CAMERA_ITEM_ID_BACK, 0, "后置摄像头");
        cameraOpt.add(CAMERA_ITEM_GROUP_ID, CAMERA_ITEM_ID_OUT_OES, 0, "输出OES纹理");
        cameraOpt.add(CAMERA_ITEM_GROUP_ID, CAMERA_ITEM_ID_OUT_FBO, 0, "输出2D纹理");
        SubMenu renderOpt = menu.addSubMenu(WHAT_DRAW_CAMERA, 0, 0, "滤镜");
        renderOpt.add(WHAT_DRAW_CAMERA, WHAT_DRAW_CAMERA_NORMAL, 0, "无滤镜");
        renderOpt.add(WHAT_DRAW_CAMERA, WHAT_DRAW_CAMERA_GRID, 0, "网格");
        renderOpt.add(WHAT_DRAW_CAMERA, WHAT_DRAW_CAMERA_SPLIT_SCREEN, 0, "分屏");
        renderOpt.add(WHAT_DRAW_CAMERA, WHAT_DRAW_CAMERA_SCALE_CIRCLE, 0, "缩放的圆");
        renderOpt.add(WHAT_DRAW_CAMERA, WHAT_DRAW_CAMERA_LUT_FILTER_1, 0, "LUT滤镜1");
        renderOpt.add(WHAT_DRAW_CAMERA, WHAT_DRAW_CAMERA_LUT_FILTER_2, 0, "LUT滤镜2");
        renderOpt.add(WHAT_DRAW_CAMERA, WHAT_DRAW_CAMERA_LUT_FILTER_3, 0, "LUT滤镜3");
        renderOpt.add(WHAT_DRAW_CAMERA, WHAT_DRAW_CAMERA_LUT_FILTER_4, 0, "LUT滤镜4");
        renderOpt.add(WHAT_DRAW_CAMERA, WHAT_DRAW_CAMERA_LUT_FILTER_5, 0, "LUT滤镜5");
        renderOpt.add(WHAT_DRAW_CAMERA, WHAT_DRAW_CAMERA_LUT_FILTER_6, 0, "LUT滤镜6");
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int itemId = item.getItemId();
        int groupId = item.getGroupId();
        if (groupId == CAMERA_ITEM_GROUP_ID) {
            switch (itemId) {
                case CAMERA_ITEM_ID_FRONT:
                    cameraCapture.changeFacing(GLCameraCapture.FACING_FRONT);
                    break;
                case CAMERA_ITEM_ID_BACK:
                    cameraCapture.changeFacing(GLCameraCapture.FACING_BACK);
                    break;
                case CAMERA_ITEM_ID_OUT_OES:
                    cameraCapture.setTexType(GLCameraCapture.TEX_TYPE_OES);
                    break;
                case CAMERA_ITEM_ID_OUT_FBO:
                    cameraCapture.setTexType(GLCameraCapture.TEX_TYPE_2D);
                    break;
            }
        } else if (groupId == GLCameraRender.WHAT_DRAW_CAMERA) {
            if(itemId > GLCameraRender.WHAT_DRAW_CAMERA){
                cameraRender.setDrawWhat(itemId);
            }
        }
        return true;
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (cameraCapture != null) {
            cameraCapture.stopCapture();
        }
        if (cameraRender != null) {
            cameraRender.stopRender();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (cameraCapture != null) {
            cameraCapture.startCapture();
        }
        if (cameraRender != null
                && surfaceView != null
                && surfaceView.getHolder().getSurface().isValid()) {
            cameraRender.startRender(surfaceView.getHolder().getSurface());
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (cameraCapture != null) {
            cameraCapture.release();
            cameraCapture = null;
        }
        if (mRootGL != null) {
            mRootGL.uninitGLEnv();
            mRootGL = null;
        }
        if (cameraRender != null) {
            cameraRender.release();
            cameraRender = null;
        }
    }
}
