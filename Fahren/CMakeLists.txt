find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED)

if(WIN32)
    set(VULKAN_LIB_SUBDIR "Lib")
else()
    set(VULKAN_LIB_SUBDIR "lib")
endif()

get_filename_component(VULKAN_SDK_LIB_DIR "${Vulkan_INCLUDE_DIR}/../${VULKAN_LIB_SUBDIR}" ABSOLUTE)

find_library(SHADERC_LIB
    NAMES shaderc_shared shaderc_sharedd shaderc_combined
    PATHS "${VULKAN_SDK_LIB_DIR}"
    NO_DEFAULT_PATH
    REQUIRED
)

find_library(SPIRV_CROSS_CORE_LIB
    NAMES spirv-cross-core spirv-cross-cored
    PATHS "${VULKAN_SDK_LIB_DIR}"
    NO_DEFAULT_PATH
    REQUIRED
)

find_library(SPIRV_CROSS_GLSL_LIB
    NAMES spirv-cross-glsl spirv-cross-glsld
    PATHS "${VULKAN_SDK_LIB_DIR}"
    NO_DEFAULT_PATH
    REQUIRED
)

message(STATUS "Vulkan include: ${Vulkan_INCLUDE_DIR}")
message(STATUS "Shaderc: ${SHADERC_LIB}")
message(STATUS "SPIRV Cross Core: ${SPIRV_CROSS_CORE_LIB}")
message(STATUS "SPIRV Cross GLSL: ${SPIRV_CROSS_GLSL_LIB}")

set(FH_SHADERC_LIBS ${SHADERC_LIB} ${SPIRV_CROSS_CORE_LIB} ${SPIRV_CROSS_GLSL_LIB})

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(TRACY_ENABLE ${FH_ENABLE_TRACY} CACHE BOOL "" FORCE)

add_subdirectory(requisites/GLAD)
add_subdirectory(requisites/GLFW)
add_subdirectory(requisites/spdlog)
add_subdirectory(requisites/tracy)

add_library(ImGui STATIC
    requisites/imgui/imgui.cpp
    requisites/imgui/imgui_draw.cpp
    requisites/imgui/imgui_widgets.cpp
    requisites/imgui/imgui_tables.cpp
    requisites/imgui/imgui_demo.cpp
)
target_include_directories(ImGui PUBLIC requisites/imgui)

add_library(ImGuizmo STATIC
    requisites/imguizmo/ImGuizmo.cpp
)
target_include_directories(ImGuizmo PUBLIC requisites/imguizmo)
target_compile_definitions(ImGuizmo PUBLIC IMGUI_DEFINE_MATH_OPERATORS)
target_link_libraries(ImGuizmo PUBLIC ImGui)

set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
add_subdirectory(requisites/glm)
set(CMAKE_WARN_DEPRECATED ON CACHE BOOL "" FORCE)

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(requisites/yaml-cpp)

set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED    OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_DOCS       OFF CACHE BOOL "" FORCE)
add_subdirectory(requisites/box2d)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/requisites/box2d/include
)

target_precompile_headers(FahrenEngine PRIVATE src/fhpch.h)

target_link_libraries(FahrenEngine PUBLIC
    ImGui
    ImGuizmo
    glad
    glfw
    OpenGL::GL
    Vulkan::Vulkan
    ${FH_SHADERC_LIBS}
    glm::glm
    spdlog::spdlog
    entt
    yaml-cpp::yaml-cpp
    Tracy::TracyClient
    box2d
)

target_compile_definitions(FahrenEngine PUBLIC
    GLFW_INCLUDE_NONE
    $<$<BOOL:${FH_ENABLE_TRACY}>:TRACY_ENABLE>
    $<$<BOOL:${FH_ENABLE_ASSERTS}>:FH_ENABLE_ASSERTS>
    $<$<BOOL:${WIN32}>:FH_PLATFORM_WINDOWS>
    $<$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>:FH_PLATFORM_LINUX>
    $<$<BOOL:${APPLE}>:FH_PLATFORM_MAC>
)