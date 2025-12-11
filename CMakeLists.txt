cmake_minimum_required(VERSION 3.10)
project(ConsoManage C)

set(CMAKE_C_STANDARD 11)

add_executable(ConsoManage
        main.c
        logic.c
        io.c
        models.c
        file.c
)
