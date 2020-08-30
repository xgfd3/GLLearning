package com.xucz.opengldemo.glsample;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.xucz.opengldemo.R;
import com.xucz.opengldemo.utils.TouchScaleHelper;

import static com.xucz.opengldemo.jni.GLSample.*;

public class GLSampleActivity extends AppCompatActivity {

    private GLRender glRender;
    private SurfaceView mSurfaceView;
    private TouchScaleHelper mTouchScaleHelper;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_glsample);

        glRender = new GLRender(this);

        // Example of a call to a native method
        mSurfaceView = findViewById(R.id.surfaceview);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                glRender.startRender(holder.getSurface());
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                glRender.stopRender();
            }
        });

        glRender.setDrawWhat(WHAT_DRAW_TRIANGLE);

        mTouchScaleHelper = new TouchScaleHelper(mSurfaceView, new TouchScaleHelper.TouchScaleListener() {
            @Override
            public void onTouchLocChanged(float x, float y) {
                glRender.changeTouchLoc(x, y);
            }

            @Override
            public void onTransformMatrixUpdated(float rotateX, float rotateY, float scaleX, float scaleY) {
                glRender.updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        menu.add(0, WHAT_DRAW_TRIANGLE, 0, "三角形");
        menu.add(0, WHAT_DRAW_TEXTUREMAP, 0, "TextureMap");
        menu.add(0, WHAT_DRAW_YUVTEXTUREMAP, 0, "YUVTextureMap");
        menu.add(0, WHAT_DRAW_VBO, 0, "VBO");
        menu.add(0, WHAT_DRAW_VAO, 0, "VAO");
        menu.add(0, WHAT_DRAW_FBO, 0, "FBO");
        menu.add(0, WHAT_DRAW_EGL, 0, "EGL");
        menu.add(0, WHAT_DRAW_TRANSFORM_FEEDBACK, 0, "Transform feedback");
        menu.add(0, WHAT_DRAW_COORDINATE_SYSTEM, 0, "Coordinate System");
        menu.add(0, WHAT_DRAW_BASIC_LIGHTING, 0, "基础光照");
        menu.add(0, WHAT_DRAW_DEPTH_TESTING, 0, "深度测试");
        menu.add(0, WHAT_DRAW_STENCIL_TESTING, 0, "模板测试");
        menu.add(0, WHAT_DRAW_BLENDING, 0, "混合");
        menu.add(0, WHAT_DRAW_INSTANCING, 0, "实例化");
        menu.add(0, WHAT_DRAW_INSTANCING3D, 0, "实例化3D");
        menu.add(0, WHAT_DRAW_PARTICLES, 0, "粒子效果");
        menu.add(0, WHAT_DRAW_SKYBOX, 0, "天空盒");
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if(item.getItemId() == WHAT_DRAW_EGL){
            startActivity(new Intent(this, EGLActivity.class));
        }else{
            glRender.setDrawWhat(item.getItemId());
        }
        return true;
    }

    @Override
    protected void onResume() {
        super.onResume();
        Surface surface = mSurfaceView.getHolder().getSurface();
        if(surface.isValid()){
           glRender.startRender(surface);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        glRender.stopRender();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        glRender.release();
    }
}
