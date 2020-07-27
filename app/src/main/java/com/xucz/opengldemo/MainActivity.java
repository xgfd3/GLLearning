package com.xucz.opengldemo;

import android.graphics.Rect;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.xucz.opengldemo.GLAPI;
import com.xucz.opengldemo.GLRender;
import com.xucz.opengldemo.R;

import java.util.HashMap;
import java.util.Map;

import javax.microedition.khronos.opengles.GL;

public class MainActivity extends AppCompatActivity {

    private GLRender glRender;
    private SurfaceView mSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        glRender = new GLRender(this);

        // Example of a call to a native method
        mSurfaceView = findViewById(R.id.surfaceview);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                Rect surfaceFrame = holder.getSurfaceFrame();
                glRender.startRender(holder.getSurface(),
                        surfaceFrame.width(), surfaceFrame.height());
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                glRender.stopRender();
            }
        });

        glRender.setDrawWhat(GLRender.WHAT_DRAW_TRIANGLE);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        menu.add(0, GLRender.WHAT_DRAW_TRIANGLE, 0, "三角形");
        menu.add(0, GLRender.WHAT_DRAW_TEXTUREMAP, 0, "TextureMap");
        menu.add(0, GLRender.WHAT_DRAW_YUVTEXTUREMAP, 0, "YUVTextureMap");
        menu.add(0, GLRender.WHAT_DRAW_VBO, 0, "VBO");
        menu.add(0, GLRender.WHAT_DRAW_VAO, 0, "VAO");
        menu.add(0, GLRender.WHAT_DRAW_FBO, 0, "FBO");
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        glRender.setDrawWhat(item.getItemId());
        return true;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        glRender.release();
    }
}
