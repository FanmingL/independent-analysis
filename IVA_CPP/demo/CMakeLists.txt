cmake_minimum_required(VERSION 3.5)
project(test_iva)
find_package(Eigen3 REQUIRED)
##iva path and parameters configuration 
set(IVA_LIB_PATH /usr/local/lib/libiva_lib.dylib)
set(IVA_INCLUDE_DIR /usr/local/include/iva)
add_definitions(-DSOURCE_NUM=2)
##project construct
add_executable(${PROJECT_NAME} main.cpp)
include_directories(${EIGEN3_INCLUDE_DIR} ${IVA_INCLUDE_DIR} )
target_link_libraries(${PROJECT_NAME}  ${IVA_LIB_PATH})



