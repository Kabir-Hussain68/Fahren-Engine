add_executable(Sandbox src/sandboxApp.cpp)

target_include_directories(Sandbox PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_precompile_headers(Sandbox REUSE_FROM FahrenEngine)

target_link_libraries(Sandbox PRIVATE FahrenEngine)