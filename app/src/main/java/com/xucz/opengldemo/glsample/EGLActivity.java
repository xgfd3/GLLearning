package com.xucz.opengldemo.glsample;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES20;
import android.opengl.GLException;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ImageView;

import com.xucz.opengldemo.R;
import com.xucz.opengldemo.jni.GLSample;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;

import static com.xucz.opengldemo.jni.GLSample.*;
import static com.xucz.opengldemo.utils.ImageUtils.*;


public class EGLActivity extends AppCompatActivity {

    private static final int SHOW_WIDTH = 933;
    private static final int SHOW_HEIGHT = 1400;



    private ImageView imageView;
    private GLSample glapi;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_egl);
        setTitle("EGL绘制");

        imageView = findViewById(R.id.imageview);
        glapi = new GLSample();

        // 初始化GL离屏环境
        glapi.initPBufferGLEnv(SHOW_WIDTH, SHOW_HEIGHT);

        // 加载图片
        loadRGBAImage(R.drawable.leg);

        // 绘制
        renderThenShow(WHAT_DRAW_EGL_NORMAL);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        menu.add(0, WHAT_DRAW_EGL_NORMAL, 0, "普通渲染");
        menu.add(0, WHAT_DRAW_EGL_MOSAIC, 0, "马赛克");
        menu.add(0, WHAT_DRAW_EGL_GRID, 0, "网格");
        menu.add(0, WHAT_DRAW_EGL_ROTATE, 0, "旋转");
        menu.add(0, WHAT_DRAW_EGL_EDGE, 0, "边缘");
        menu.add(0, WHAT_DRAW_EGL_ENLARGE, 0, "放大");
        menu.add(0, WHAT_DRAW_EGL_UNKNOW, 0, "未知");
        menu.add(0, WHAT_DRAW_EGL_DEFORMATION, 0, "形变");
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        renderThenShow(item.getItemId());
        return true;
    }

    private void renderThenShow(int what) {
        glapi.draw(what);
        Bitmap bitmapFromGLSurface = createBitmapFromGLSurface(0, 0, SHOW_WIDTH, SHOW_HEIGHT);
        imageView.setImageBitmap(bitmapFromGLSurface);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        glapi.uninitGLEnv();
    }

    private Bitmap createBitmapFromGLSurface(int x, int y, int w, int h) {
        int bitmapBuffer[] = new int[w * h];
        int bitmapSource[] = new int[w * h];
        IntBuffer intBuffer = IntBuffer.wrap(bitmapBuffer);
        intBuffer.position(0);
        try {
            GLES20.glReadPixels(x, y, w, h, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE,
                    intBuffer);
            int offset1, offset2;
            for (int i = 0; i < h; i++) {
                offset1 = i * w;
                offset2 = (h - i - 1) * w;
                for (int j = 0; j < w; j++) {
                    int texturePixel = bitmapBuffer[offset1 + j];
                    int blue = (texturePixel >> 16) & 0xff;
                    int red = (texturePixel << 16) & 0x00ff0000;
                    int pixel = (texturePixel & 0xff00ff00) | red | blue;
                    bitmapSource[offset2 + j] = pixel;
                }
            }
        } catch (GLException e) {
            return null;
        }
        return Bitmap.createBitmap(bitmapSource, w, h, Bitmap.Config.ARGB_8888);
    }


    private void loadRGBAImage(int resId) {
        InputStream is = getResources().openRawResource(resId);
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
