cmake_minimum_required(VERSION 3.1.0)
project(tetris)

include_directories(vendor/glad/include/)
include_directories(vendor/stb/)

include_directories(vendor/freetype-2.10.4/include)
link_directories(vendor/freetype-2.10.4/win64/)

include_directories(vendor/glfw-3.3.4/include/)
link_directories(vendor/glfw-3.3.4/lib-vc2019/)

include_directories(vendor/openal-soft-1.21.1-bin/include)
link_directories(vendor/openal-soft-1.21.1-bin/libs/Win64)

include_directories(vendor/libsndfile/include)
link_directories(vendor/libsndfile/lib)

set(SOURCES src/main.c src/tetris.c src/shader.c src/renderer.c src/window.c src/painter.c src/audio.c)
set(GLAD_SOURCES vendor/glad/src/glad.c)
add_executable(tetris ${SOURCES} ${GLAD_SOURCES})
set_property(TARGET tetris PROPERTY C_STANDARD 11)
if(WIN32)
    target_compile_options(tetris PRIVATE)
    target_link_libraries(tetris glfw3dll freetype OpenAL32 libsndfile-1)
endif(WIN32)
if(UNIX)
    target_compile_options(tank PRIVATE)
    target_link_libraries(tank SDL2 SDL2_image dl pthread)
endif(UNIX)
