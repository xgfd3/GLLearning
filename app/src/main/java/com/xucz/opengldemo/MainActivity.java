package com.xucz.opengldemo;

import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.xucz.opengldemo.camera.CameraActivity;
import com.xucz.opengldemo.glsample.GLSampleActivity;

/**
 * Function:
 *
 * @author xucz
 * @since 2020/8/23
 */
public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void gotoGLSamplesAty(View view){
        startActivity(new Intent(this, GLSampleActivity.class));
    }

    public void gotoCameraAty(View view){
        startActivity(new Intent(this, CameraActivity.class));
    }
}
