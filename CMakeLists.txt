cmake_minimum_required(VERSION 3.16)

project(technopark-highload-web-server)

include_directories(./include)

add_executable(hw2
            main.c 
            ./src/sender.c
            ./src/pthreads.c
            ./src/task.c
            ./src/receiver.c
            ./include/sender.h
            ./include/http.h
            ./include/pthreads.h
            ./include/task.h
            ./include/receiver.h
            )

target_link_libraries(hw2 pthread)
