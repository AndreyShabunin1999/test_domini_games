package com.fortune.mouse.chesee.test_domini_games

import android.opengl.GLES20
import android.opengl.GLSurfaceView
import android.view.MotionEvent
import android.view.View
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class MyGLRenderer : GLSurfaceView.Renderer, View.OnTouchListener {

    private var angleX: Float = 0.0f
    private var angleY: Float = 0.0f
    private var angleZ: Float = 0.0f

    // Новые переменные для текущих углов
    private var currentAngleX: Float = 0.0f
    private var currentAngleY: Float = 0.0f
    private var currentAngleZ: Float = 0.0f

    private var startX: Float = 0.0f
    private var startY: Float = 0.0f
    private var startZ: Float = 0.0f

    external fun renderFrame(angleX: Float, angleY: Float, angleZ: Float)

    init {
        System.loadLibrary("native-lib")
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {

    }

    override fun onDrawFrame(gl: GL10?) {
        renderFrame(currentAngleX, currentAngleY, currentAngleZ)
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        GLES20.glViewport(0, 0, width, height)
    }

    fun setRotationAngles(angleX: Float, angleY: Float, angleZ: Float) {
        this.angleX = angleX
        this.angleY = angleY
        this.angleZ = angleZ
    }

    override fun onTouch(view: View?, event: MotionEvent?): Boolean {
        when (event?.action) {
            MotionEvent.ACTION_DOWN -> {
                startX = event.x
                startY = event.y
                startZ = currentAngleZ
            }
            MotionEvent.ACTION_MOVE -> {
                val currentX = event.x
                val currentY = event.y
                val currentZ = 0.0f
                val deltaX = currentX - startX
                val deltaY = currentY - startY
                val deltaZ = currentZ - startZ

                currentAngleX += deltaY * TOUCH_SCALE_FACTOR
                currentAngleY += deltaX * TOUCH_SCALE_FACTOR
                currentAngleZ += deltaZ * TOUCH_SCALE_FACTOR

                setRotationAngles(currentAngleX, currentAngleY, currentAngleZ)
                (view as GLSurfaceView).requestRender()
            }
            MotionEvent.ACTION_UP -> {
                angleX = currentAngleX
                angleY = currentAngleY
                angleZ = currentAngleZ
            }
        }
        return true
    }

    external fun setScreenSize(width: Int, height: Int)

    companion object {
        private const val TOUCH_SCALE_FACTOR: Float = 0.0002f
    }
}
