cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0072 NEW)

project(PixGL VERSION 0.1.0)
find_package(OpenGL REQUIRED)

find_package(glfw3 REQUIRED)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
)

add_executable(
	PixGL
	src/old/main.cpp
    src/glad.c
)

target_link_libraries(
	PixGL
	glfw
)
