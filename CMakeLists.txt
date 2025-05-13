cmake_minimum_required(VERSION 3.20)
project(peb_walk LANGUAGES C CXX ASM_MASM)

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/*.asm
    src/*.h
)

add_executable(peb_walk ${SOURCES})

source_group("src" FILES ${SOURCES})
