package com.xucz.opengldemo.utils;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

/**
 * Function:
 *
 * @author xucz
 * @since 2020/8/30
 */
public class ImageUtils {

    // 图像数据格式，和ImageUtils.h的类型一一对应
    public static final int IMAGE_FORMAT_RGBA = 0x01;
    public static final int IMAGE_FORMAT_NV21 = 0x02;
    public static final int IMAGE_FORMAT_NV12 = 0x03;
    public static final int IMAGE_FORMAT_I420 = 0x04;


    public static ImageData loadRGBAImage(Context context, int resId, int index) {
        InputStream is = context.getResources().openRawResource(resId);
        Bitmap bitmap;
        try {
            bitmap = BitmapFactory.decodeStream(is);
            if (bitmap != null) {
                int bytes = bitmap.getByteCount();
                ByteBuffer buf = ByteBuffer.allocate(bytes);
                bitmap.copyPixelsToBuffer(buf);
                byte[] byteArray = buf.array();
                return new ImageData(bitmap.getWidth(), bitmap.getHeight(), IMAGE_FORMAT_RGBA, byteArray);
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
        return null;
    }

    public static class ImageData{
        public int width;
        public int height;
        public int format;
        public byte[] data;

        public ImageData(int width, int height, int format, byte[] data) {
            this.width = width;
            this.height = height;
            this.format = format;
            this.data = data;
        }
    }
}
