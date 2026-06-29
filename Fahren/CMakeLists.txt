find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(requisites/GLAD)
add_subdirectory(requisites/GLFW)
add_subdirectory(requisites/spdlog)

add_library(ImGui STATIC
    requisites/imgui/imgui.cpp
    requisites/imgui/imgui_draw.cpp
    requisites/imgui/imgui_widgets.cpp
    requisites/imgui/imgui_tables.cpp
    requisites/imgui/imgui_demo.cpp
)
target_include_directories(ImGui PUBLIC requisites/imgui)

set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
add_subdirectory(requisites/glm)
set(CMAKE_WARN_DEPRECATED ON CACHE BOOL "" FORCE)

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(requisites/yaml-cpp)

add_library(entt INTERFACE)
target_include_directories(entt INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/requisites/entt/include
)

file(GLOB_RECURSE FAHREN_SOURCES
    "src/*.cpp"
    "src/*.h"
    "requisites/stb_image/*.cpp"
    "requisites/stb_image/*.h"
)

list(REMOVE_ITEM FAHREN_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/windows/windowsPlatformUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/linux/linuxPlatformUtils.cpp
)

add_library(FahrenEngine STATIC ${FAHREN_SOURCES})

if(WIN32)
    target_sources(FahrenEngine PRIVATE
        src/Platform/windows/windowsPlatformUtils.cpp
    )
elseif(UNIX)
    target_sources(FahrenEngine PRIVATE
        src/Platform/linux/linuxPlatformUtils.cpp
    )
endif()


target_include_directories(FahrenEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/requisites/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/requisites/stb_image 
)

target_precompile_headers(FahrenEngine PRIVATE src/fhpch.h)

target_link_libraries(FahrenEngine PUBLIC
    ImGui
    glad
    glfw
    OpenGL::GL
    glm::glm
    spdlog::spdlog
    entt
    yaml-cpp::yaml-cpp
)

target_compile_definitions(FahrenEngine PUBLIC
    GLFW_INCLUDE_NONE
    $<$<BOOL:${FH_ENABLE_ASSERTS}>:FH_ENABLE_ASSERTS>
    $<$<BOOL:${WIN32}>:FH_PLATFORM_WINDOWS>
    $<$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>:FH_PLATFORM_LINUX>
    $<$<BOOL:${APPLE}>:FH_PLATFORM_MAC>
)