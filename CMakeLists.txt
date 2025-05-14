cmake_minimum_required(VERSION 3.20)
project(peb_walk LANGUAGES C CXX ASM_MASM)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/*.asm
    src/*.h
    src/*.inl
)

add_executable(peb_walk ${SOURCES})

source_group("src" FILES ${SOURCES})

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_compile_definitions(peb_walk PRIVATE _WIN64)
endif()