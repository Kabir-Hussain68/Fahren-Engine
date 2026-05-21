cmake_minimum_required(VERSION 3.22)

project(OpenGLEngine LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(FH_ENABLE_ASSERTS "Enable asserts" ON)

find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(requisites/GLAD)
add_subdirectory(requisites/imgui)
add_subdirectory(requisites/GLFW)
add_subdirectory(requisites/spdlog)

set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
add_subdirectory(requisites/glm)
set(CMAKE_WARN_DEPRECATED ON CACHE BOOL "" FORCE)

add_library(FahrenEngine STATIC
    src/Fahren.h
    src/fhpch.h

    src/Engine/core/application.cpp
    src/Engine/core/log.cpp
    src/Engine/core/window.cpp
    src/Engine/core/layer.cpp
    src/Engine/core/layerStack.cpp
    src/Engine/core/entryPoint.h

    src/Engine/events/events.h
    src/Engine/events/applicationEvent.h
    src/Engine/events/mouseEvent.h
    src/Engine/events/keyEvent.h

    src/Platform/windows/windowWindow.cpp
    src/Platform/OpenGL/imGuiOpenGLRenderer.cpp
    src/Engine/imGui/imGuiLayer.cpp
)

target_include_directories(FahrenEngine PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/requisites/imgui
)

target_precompile_headers(FahrenEngine PRIVATE src/fhpch.h)

target_link_libraries(FahrenEngine PUBLIC
    ImGui
    glad
    glfw
    OpenGL::GL
    glm::glm
    spdlog::spdlog
)

target_compile_definitions(FahrenEngine PUBLIC
    GLFW_INCLUDE_NONE
    $<$<BOOL:${FH_ENABLE_ASSERTS}>:FH_ENABLE_ASSERTS>
    $<$<BOOL:${WIN32}>:FH_PLATFORM_WINDOWS>
    $<$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>:FH_PLATFORM_LINUX>
    $<$<BOOL:${APPLE}>:FH_PLATFORM_MAC>
)

add_subdirectory(sandbox)