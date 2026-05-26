cmake_minimum_required(VERSION 3.22)

project(OpenGLEngine LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(FH_ENABLE_ASSERTS "Enable asserts" ON)

add_subdirectory(Fahren)
add_subdirectory(Sandbox)