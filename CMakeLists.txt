cmake_minimum_required(VERSION 3.10)
project(IntelliEditorTest C)

include_directories(.)
include_directories(src)

add_executable(intelli_test 
	src/main.c 
    src/checkers.c 
    src/parser.c 
    cJSON.c
              )

target_link_libraries(intelli_test m)
                
