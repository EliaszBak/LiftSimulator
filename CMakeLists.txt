cmake_minimum_required(VERSION 3.22)

project(Hello)
## Create main executable
# imgui
include_directories("imgui")
include_directories("imgui/backends")
add_executable(main main.cpp)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS})
target_link_libraries(main glfw ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

file(GLOB SRC_IMGUI imgui/*.cpp)
set(SRC_IMGUI_BACKEND imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_sdl.cpp)
target_sources(main PRIVATE ${SRC_IMGUI} ${SRC_IMGUI_BACKEND})
