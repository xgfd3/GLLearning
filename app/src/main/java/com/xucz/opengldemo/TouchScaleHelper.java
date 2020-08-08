package com.xucz.opengldemo;

import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;

/**
 * 触摸缩放事件监听
 *
 * @author xucz
 * @since 2020/8/8
 */
public class TouchScaleHelper implements View.OnTouchListener, ScaleGestureDetector.OnScaleGestureListener {

    private final static float TOUCH_SCALE_FACTOR = 180.0f / 320;

    private TouchScaleListener touchScaleListener;

    private float mPreviousY;
    private float mPreviousX;
    private int mXAngle;
    private int mYAngle;

    private ScaleGestureDetector mScaleGestureDetector;
    private float mPreScale = 1.0f;
    private float mCurScale = 1.0f;
    private long mLastMultiTouchTime;

    public TouchScaleHelper(View view, TouchScaleListener touchScaleListener){
        this.touchScaleListener = touchScaleListener;
        view.setOnTouchListener(this);
        mScaleGestureDetector = new ScaleGestureDetector(view.getContext(), this);
    }

    public void dealClickEvent(MotionEvent e) {
        float touchX = -1, touchY = -1;
        switch (e.getAction()) {
            case MotionEvent.ACTION_UP:
                touchX = e.getX();
                touchY = e.getY();
            {
                //点击
                if(touchScaleListener != null){
                    touchScaleListener.onTouchLocChanged(touchX, touchY);
                }

            }
            break;
            default:
                break;
        }
    }

    public void consumeTouchEvent(MotionEvent e) {
        dealClickEvent(e);
        float touchX = -1, touchY = -1;
        switch (e.getAction()) {
            case MotionEvent.ACTION_MOVE:
                touchX = e.getX();
                touchY = e.getY();
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                touchX = -1;
                touchY = -1;
                break;
            default:
                break;
        }

        //滑动、触摸
        if(touchScaleListener != null){
            touchScaleListener.onTouchLocChanged(touchX, touchY);
        }

        //点击
        switch (e.getAction()) {
            case MotionEvent.ACTION_MOVE:
                //touchX = e.getX();
                //touchY = e.getY();
                break;
            case MotionEvent.ACTION_UP:
                break;
            case MotionEvent.ACTION_CANCEL:
                break;
            default:
                break;
        }

    }

    @Override
    public boolean onTouch(View v, MotionEvent e) {
        if (e.getPointerCount() == 1) {
            consumeTouchEvent(e);
            long currentTimeMillis = System.currentTimeMillis();
            if (currentTimeMillis - mLastMultiTouchTime > 200)
            {
                float y = e.getY();
                float x = e.getX();
                switch (e.getAction()) {
                    case MotionEvent.ACTION_MOVE:
                        float dy = y - mPreviousY;
                        float dx = x - mPreviousX;
                        mYAngle += dx * TOUCH_SCALE_FACTOR;
                        mXAngle += dy * TOUCH_SCALE_FACTOR;
                }
                mPreviousY = y;
                mPreviousX = x;

                if(touchScaleListener != null){
                    touchScaleListener.onTransformMatrixUpdated(mXAngle, mYAngle, mCurScale, mCurScale);
                }
            }

        } else {
            mScaleGestureDetector.onTouchEvent(e);
        }
        return true;
    }

    @Override
    public boolean onScale(ScaleGestureDetector detector) {
        float preSpan = detector.getPreviousSpan();
        float curSpan = detector.getCurrentSpan();
        if (curSpan < preSpan) {
            mCurScale = mPreScale - (preSpan - curSpan) / 200;
        } else {
            mCurScale = mPreScale + (curSpan - preSpan) / 200;
        }
        mCurScale = Math.max(0.05f, Math.min(mCurScale, 80.0f));
        if(touchScaleListener != null){
            touchScaleListener.onTransformMatrixUpdated(mXAngle, mYAngle, mCurScale, mCurScale);
        }

        return false;
    }

    @Override
    public boolean onScaleBegin(ScaleGestureDetector detector) {
        return true;
    }

    @Override
    public void onScaleEnd(ScaleGestureDetector detector) {
        mPreScale = mCurScale;
        mLastMultiTouchTime = System.currentTimeMillis();
    }

    public interface TouchScaleListener{
        void onTouchLocChanged(float x, float y);
        void onTransformMatrixUpdated(float rotateX, float rotateY, float scaleX, float scaleY);
    }


}
