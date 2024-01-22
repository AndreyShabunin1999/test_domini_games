//
// Created by Андрей Шабунин on 20.01.2024.
//
#include "main.h"
#include <jni.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

EGLDisplay display;
EGLSurface surface;
GLuint vbo;
GLuint ibo;  // Добавленный буфер для индексов
GLuint vertexShader, fragmentShader, shaderProgram;

GLfloat screenWidth = 0.0f;
GLfloat screenHeight = 0.0f;

// Определение структуры матрицы
struct Matrix4 {
    float data[16];
};

/*
void rotateMatrix(float angle, float x, float y, float z, Matrix4& matrix) {
    glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(x, y, z));

    // Копируем матрицу из glm в нашу структуру
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            matrix.data[i * 4 + j] = modelMatrix[j][i];
        }
    }
}
 */

void rotateMatrix(float angle, float x, float y, float z, glm::mat4& matrix) {
    matrix = glm::rotate(matrix, glm::radians(angle), glm::vec3(x, y, z));
}


// Метод для установки размеров экрана из Kotlin
extern "C" JNIEXPORT void JNICALL Java_com_fortune_mouse_chesee_test_1domini_1games_MyGLRenderer_setScreenSize(
        JNIEnv *env, jobject thiz, jint width, jint height) {
    screenWidth = static_cast<GLfloat>(width);
    screenHeight = static_cast<GLfloat>(height);
}

extern "C" JNIEXPORT void JNICALL Java_com_fortune_mouse_chesee_test_1domini_1games_MyGLRenderer_renderFrame(
        JNIEnv *env, jobject thiz, jfloat angle_x, jfloat angle_y, jfloat angle_z) {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    GLfloat cubeVertices[] = {
            // Передняя грань
            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

            // Задняя грань
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,

            // Верхняя грань
            -0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,

            // Нижняя грань
            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            // Левая грань
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            // Правая грань
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
    };

    GLuint indices[] = {
            0, 1, 2,   // Первый треугольник (передняя грань)
            0, 2, 3,   // Второй треугольник (передняя грань)

            4, 5, 6,   // Первый треугольник (задняя грань)
            4, 6, 7,   // Второй треугольник (задняя грань)

            8, 9, 10,  // Первый треугольник (верхняя грань)
            8, 10, 11, // Второй треугольник (верхняя грань)

            12, 13, 14, // Первый треугольник (нижняя грань)
            12, 14, 15, // Второй треугольник (нижняя грань)

            16, 17, 18, // Первый треугольник (левая грань)
            16, 18, 19, // Второй треугольник (левая грань)

            20, 21, 22, // Первый треугольник (правая грань)
            20, 22, 23  // Второй треугольник (правая грань)
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Проверка ошибок EGL
    EGLint eglError = eglGetError();
    if (eglError != EGL_SUCCESS) {
        __android_log_print(ANDROID_LOG_ERROR, "EGL", "Ошибка EGL перед созданием шейдеров: %d", eglError);
    }

    const char* vertexShaderCode = R"(
        attribute vec3 vPosition;
        uniform mat4 uMVPMatrix;
        void main() {
            gl_Position = uMVPMatrix * vec4(vPosition, 1.0);
        }
    )";

    const char* fragmentShaderCode = R"(
        precision mediump float;
        void main() {
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    )";

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertexShader);

    // Проверка ошибок компиляции вершинного шейдера
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
        __android_log_print(ANDROID_LOG_ERROR, "OpenGL", "Ошибка компиляции вершинного шейдера: %s", infoLog);
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
    glCompileShader(fragmentShader);

    // Проверка ошибок компиляции фрагментного шейдера
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
        __android_log_print(ANDROID_LOG_ERROR, "OpenGL", "Ошибка компиляции фрагментного шейдера: %s", infoLog);
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    GLuint positionAttrib = glGetAttribLocation(shaderProgram, "vPosition");
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(positionAttrib);

/*
    // Создаем матрицу модели
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // Вращаем куб вокруг осей X, Y и Z на переданные углы
    rotateMatrix(angle_x, 1.0f, 0.0f, 0.0f, modelMatrix);
    rotateMatrix(angle_y, 0.0f, 1.0f, 0.0f, modelMatrix);
    rotateMatrix(angle_z, 0.0f, 0.0f, 1.0f, modelMatrix);
    */

  // Создаем кватернион из углов поворота
    glm::quat rotationQuat = glm::quat(glm::vec3(angle_x, angle_y, angle_z));

    // Применяем вращение к матрице модели
    glm::mat4 modelMatrix = glm::mat4_cast(rotationQuat);

    // Создаем матрицу проекции (в данном примере используется перспективная проекция)
    // В функции renderFrame измените создание проекционной матрицы следующим образом:
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(95.0f), screenWidth / screenHeight, 0.1f, 10.0f);

    // Создаем матрицу вида (в данном примере смотрим сзади)
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 mvpMatrix;
    memcpy(glm::value_ptr(mvpMatrix), glm::value_ptr(modelMatrix), sizeof(float) * 16);

    mvpMatrix = projectionMatrix * viewMatrix * mvpMatrix;

    GLint mvpMatrixHandle = glGetUniformLocation(shaderProgram, "uMVPMatrix");

    // Передаем матрицу в шейдер
    glUniformMatrix4fv(mvpMatrixHandle, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    // Используем индексы для отрисовки
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Обмен буферами (если используется двойная буферизация)
    eglSwapBuffers(display, surface);

    // Проверка ошибок OpenGL после отрисовки
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        __android_log_print(ANDROID_LOG_ERROR, "OpenGL", "OpenGL error: %d", error);
    }

    // Удаление буфера вершин и шейдеров
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);  // Удаляем буфер индексов
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(shaderProgram);
}
