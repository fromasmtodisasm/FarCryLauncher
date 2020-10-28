cmake_minimum_required(VERSION 3.16)
project(FarCryLauncher)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(FarCryLauncher WIN32
        CryVersion.h
        FarCryLauncher.cpp
        IGame.h)
