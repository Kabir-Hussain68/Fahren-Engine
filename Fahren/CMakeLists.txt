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

add_library(FahrenEngine STATIC ${FAHREN_SOURCES})

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
)

target_compile_definitions(FahrenEngine PUBLIC
    GLFW_INCLUDE_NONE
    $<$<BOOL:${FH_ENABLE_ASSERTS}>:FH_ENABLE_ASSERTS>
    $<$<BOOL:${WIN32}>:FH_PLATFORM_WINDOWS>
    $<$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>:FH_PLATFORM_LINUX>
    $<$<BOOL:${APPLE}>:FH_PLATFORM_MAC>
)