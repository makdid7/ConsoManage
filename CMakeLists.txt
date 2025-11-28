cmake_minimum_required(VERSION 4.0)
project(ConsoManage)

set(CMAKE_CXX_STANDARD 20)

add_executable(Team6_Release3_Code.c
        main.c
        logic.h
        logic.c
        io.c
        io.h
        models.c
        models.h
        fileio.c
        fileio.h)
