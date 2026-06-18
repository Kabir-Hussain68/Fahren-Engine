file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.h"
)

add_executable(Editor ${EDITOR_SOURCES})

target_precompile_headers(Editor REUSE_FROM FahrenEngine)

target_link_libraries(Editor PRIVATE FahrenEngine)