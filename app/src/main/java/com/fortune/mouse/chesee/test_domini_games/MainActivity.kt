package com.fortune.mouse.chesee.test_domini_games

import android.app.Activity
import android.graphics.Point
import android.opengl.GLSurfaceView
import android.os.Bundle

class MainActivity : Activity() {

    private lateinit var glSurfaceView: GLSurfaceView
    private lateinit var renderer: MyGLRenderer

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // Загрузка библиотеки
        System.loadLibrary("native-lib")

        // Создаем GLSurfaceView
        glSurfaceView = GLSurfaceView(this)

        // Устанавливаем OpenGL ES версию
        glSurfaceView.setEGLContextClientVersion(2)

        glSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0)

        // Создаем экземпляр MyGLRenderer
        renderer = MyGLRenderer()

        // Устанавливаем Renderer
        glSurfaceView.setRenderer(renderer)

        // Устанавливаем обработчик касаний
        glSurfaceView.setOnTouchListener(renderer)

        // Устанавливаем GLSurfaceView как основное содержимое активности
        setContentView(glSurfaceView)

        // Получаем размеры экрана и передаем их в рендерер
        val display = windowManager.defaultDisplay
        val size = Point()
        display.getSize(size)
        renderer.setScreenSize(size.x, size.y)
    }

    override fun onResume() {
        super.onResume()
        glSurfaceView.onResume()
    }

    override fun onPause() {
        super.onPause()
        glSurfaceView.onPause()
    }
}
