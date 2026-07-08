file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.h"
)

add_executable(Editor ${EDITOR_SOURCES})

target_precompile_headers(Editor REUSE_FROM FahrenEngine)

target_link_libraries(Editor PRIVATE FahrenEngine)

if(WIN32)
    find_package(Vulkan REQUIRED)
    get_filename_component(VULKAN_SDK_BIN_DIR "${Vulkan_INCLUDE_DIR}/../Bin" ABSOLUTE)

    set(FH_SHADERC_DLL
        "$<$<CONFIG:Debug>:${VULKAN_SDK_BIN_DIR}/shaderc_sharedd.dll>$<$<NOT:$<CONFIG:Debug>>:${VULKAN_SDK_BIN_DIR}/shaderc_shared.dll>"
    )
    set(FH_SPIRV_CROSS_DLL
        "$<$<CONFIG:Debug>:${VULKAN_SDK_BIN_DIR}/spirv-cross-c-sharedd.dll>$<$<NOT:$<CONFIG:Debug>>:${VULKAN_SDK_BIN_DIR}/spirv-cross-c-shared.dll>"
    )

    add_custom_command(TARGET Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FH_SHADERC_DLL}" $<TARGET_FILE_DIR:Editor>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FH_SPIRV_CROSS_DLL}" $<TARGET_FILE_DIR:Editor>
        COMMENT "Copying Vulkan shaderc/SPIRV-Cross DLLs next to Editor.exe"
    )
endif()