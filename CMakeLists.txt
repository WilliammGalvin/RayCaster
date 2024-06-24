cmake_minimum_required(VERSION 3.28)
project(RayCaster C)

set(CMAKE_C_STANDARD 23)

# Find freeglut package (includes OpenGL and glut)
find_package(GLUT REQUIRED)

# Add executable target
add_executable(RayCaster main.c)

# Include directories for GLUT headers
include_directories("/opt/homebrew/include")

# Link against freeglut libraries
link_directories("/opt/homebrew/lib")
target_link_libraries(RayCaster PRIVATE GLUT::GLUT)

# Link against OpenGL framework
find_library(OpenGL_LIBRARY OpenGL)
if(OpenGL_LIBRARY)
    target_link_libraries(RayCaster PRIVATE ${OpenGL_LIBRARY})
else()
    message(FATAL_ERROR "OpenGL library not found")
endif()
