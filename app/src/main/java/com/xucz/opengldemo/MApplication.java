package com.xucz.opengldemo;

import android.app.Application;

/**
 * Function:
 *
 * @author xucz
 * @since 2020/8/30
 */
public class MApplication extends Application {

    public static Application sApplication;

    @Override
    public void onCreate() {
        super.onCreate();
        sApplication = this;
    }

    public static Application getApplication() {
        return sApplication;
    }
}
