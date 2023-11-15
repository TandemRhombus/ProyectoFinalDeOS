cmake_minimum_required(VERSION 3.26)
project(ProyectoFinalDeOS)

set(CMAKE_CXX_STANDARD 17)

add_executable(ProyectoFinalDeOS main.cpp
        MemoryManager.h
        Process.h
        Scheduler.h
        UserInterface.h
        Statistics.h
        Process.cpp
        MemoryManager.cpp
        Scheduler.cpp
        UserInterface.cpp
        Statistics.cpp)
