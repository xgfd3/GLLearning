package com.xucz.opengldemo;

import android.view.Surface;

/**
 * 描述：
 *
 * @author 创建人 ：xucz
 * @version 1.0
 * @createTime 创建时间 ：2019-06-22
 * @modifyBy 修改人 ：
 * @modifyTime 修改时间 ：
 * @modifyMemo 修改备注：
 */
public class GLAPI {

    static {
        System.loadLibrary("native-lib");
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public static native void setSurface(Surface surface);

}
