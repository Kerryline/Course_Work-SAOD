cmake_minimum_required(VERSION 3.10)
project(CourseWork)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(coursework 
    src/main.cpp 
    src/database.cpp 
    src/sort.cpp 
    src/display.cpp 
    src/queue.cpp 
    src/search.cpp 
    src/tree.cpp 
    src/shannon.cpp
)