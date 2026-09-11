cmake_minimum_required(VERSION 4.4)
project(BPengine C)

set(CMAKE_C_STANDARD 99)

add_executable(BPengine src/main.c
        bpengine.h
        user_main.c
        src/renderer.c
        headers/typedefs.h
        headers/renderer.h
        headers/app.h
        headers/input_manager.h
        src/input_manager.c
        headers/config.h
        headers/debug.h
        src/debug.c
        src/time.c
        headers/time.h
)
