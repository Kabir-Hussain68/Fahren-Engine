cmake_minimum_required(VERSION 3.22)

project(OpenGLEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)

add_executable(OpenGLEngine
    src/main.cpp
    src/Fahren.h
    src/Engine/core/application.cpp
    src/Engine/core/log.cpp
    src/Engine/events/events.h
    src/Engine/events/applicationEvent.h
    src/Engine/events/mouseEvent.h
    src/Engine/events/keyEvent.h
    src/Engine/core/window.cpp
    src/Platform/windows/linuxWindow.cpp
    src/Engine/core/layer.cpp
    src/Engine/core/layerStack.cpp
)

target_include_directories(OpenGLEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_precompile_headers(OpenGLEngine PRIVATE
    src/fhpch.h
)

target_link_libraries(OpenGLEngine PRIVATE
    OpenGL::GL
    glfw
    glm::glm
    spdlog::spdlog
)