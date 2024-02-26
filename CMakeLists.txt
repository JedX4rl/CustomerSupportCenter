cmake_minimum_required(VERSION 3.26)
project(lab9 C)

set(CMAKE_C_STANDARD 11)

add_executable(lab9 main.c
        bin_heap.h
        utility.h
        utility.c
        priority_queue.c
        priority_queue.h
        binary_heap.c
        binary_heap.h
        pq_test.c)
