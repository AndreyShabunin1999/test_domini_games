# CMakeLists.txt
cmake_minimum_required(VERSION 3.10.2)

project(test_domini_games)

# Включаем поддержку языка C++11
set(CMAKE_CXX_STANDARD 11)

# Подключаем библиотеки OpenGL ES
find_library(log-lib log)
find_library(android-lib android)
find_library(EGL-lib EGL)
find_library(GLESv2-lib GLESv2)

# Добавляем путь к исходникам
set(SRC_DIR src/main/cpp)

# Создаем исполняемый файл
add_library(native-lib SHARED ${SRC_DIR}/main.cpp)

target_include_directories(native-lib PRIVATE ${SRC_DIR}/glm )

# Связываем библиотеки
target_link_libraries(native-lib ${log-lib} ${android-lib} ${EGL-lib} ${GLESv2-lib})

