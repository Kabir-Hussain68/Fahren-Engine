file(GLOB_RECURSE SANDBOX_SOURCES
    "src/*.cpp"
    "src/*.h"
)

add_executable(Sandbox ${SANDBOX_SOURCES})

target_precompile_headers(Sandbox REUSE_FROM FahrenEngine)

target_link_libraries(Sandbox PRIVATE FahrenEngine)