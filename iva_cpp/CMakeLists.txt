cmake_minimum_required(VERSION 3.8)
project(iva)
set(src_lists main.cpp iva.cpp)
set(proto_buf_dir ${CMAKE_CURRENT_SOURCE_DIR}/proto/iva.proto)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
find_package(Eigen3 REQUIRED)
find_package(ProtoBuf REQUIRED)
#find_package(Boost REQUIRED COMPONENTS
#	  		system
#	    	thread
#			)
include_directories(${EIGEN3_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIR})
message(STATUS "proto file dirctory is "  ${proto_buf_dir})
message(STATUS "proto libs dirctory is "  ${PROTOBUF_LIBRARIES})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${proto_buf_dir}) 
add_executable(${PROJECT_NAME} ${src_lists} ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES})
set(lib_src_lists iva.cpp ${PROTO_SRCS} ${PROTO_HDRS})
add_library(${PROJECT_NAME}_lib SHARED ${lib_src_lists})
target_link_libraries(${PROJECT_NAME}_lib ${PROTOBUF_LIBRARIES})


