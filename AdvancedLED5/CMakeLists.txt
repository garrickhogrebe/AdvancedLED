cmake_minimum_required(VERSION 3.20)
project(AdvancedLED5)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(AdvancedLED5
        AdvancedLED5.cpp
        AdvancedLED5.h
        Animations.cpp
        Animations.h
        AudioSampling.cpp
        AudioSampling.h
        LEDHandler.cpp
        LEDHandler.h
        Loader.cpp
        Loader.h
        Utilities.cpp
        Utilities.h)
