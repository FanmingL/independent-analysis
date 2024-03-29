set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
add_definitions(-DSOURCE_NUM=2)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)
function(modified_protobuf_generate_cpp output_dir proto_srcs proto_hdrs)
  if(NOT ARGN)
    message(SEND_ERROR "Error: rrts_protobuf_generate_cpp() called without any proto files")
    return()
  endif()

  if(PROTOBUF_GENERATE_CPP_APPEND_PATH)
    # Create an include path for each file specified
    foreach(fil ${ARGN})
      get_filename_component(abs_fil ${fil} ABSOLUTE)
      get_filename_component(abs_path ${abs_fil} PATH)
      list(FIND _protoc_include ${abs_path} _contains_already)
      if(${_contains_already} EQUAL -1)
        list(APPEND _protoc_include -I ${abs_path})
      endif()
    endforeach()
  else()
    set(_protoc_include -I ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  if(DEFINED PROTOBUF_IMPORT_DIRS)
    foreach(dir ${PROTOBUF_IMPORT_DIRS})
      get_filename_component(abs_path ${dir} ABSOLUTE)
      list(FIND _protoc_include ${abs_path} _contains_already)
      if(${_contains_already} EQUAL -1)
        list(APPEND _protoc_include -I ${abs_path})
      endif()
    endforeach()
  endif()

  set(${proto_srcs})
  set(${proto_hdrs})
  foreach(fil ${ARGN})
    get_filename_component(abs_fil ${fil} ABSOLUTE)
    get_filename_component(fil_we ${fil} NAME_WE)
    
    list(APPEND ${proto_srcs} "${output_dir}/${fil_we}.pb.cc")
    list(APPEND ${proto_hdrs} "${output_dir}/${fil_we}.pb.h")

    add_custom_command(
      OUTPUT "${output_dir}/${fil_we}.pb.cpp"
             "${output_dir}/${fil_we}.pb.h"
      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS --cpp_out ${output_dir} ${_protoc_include} ${abs_fil}
      DEPENDS ${abs_fil}
      COMMENT "Running C++ protocol buffer compiler on ${fil}"
      VERBATIM )
  endforeach()

  set_source_files_properties(${${proto_srcs}} ${${proto_hdrs}} PROPERTIES GENERATED TRUE)
  set(${proto_srcs} ${${proto_srcs}} PARENT_SCOPE)
  set(${proto_hdrs} ${${proto_hdrs}} PARENT_SCOPE)
endfunction()


project(iva)
set(proto_name iva)
set(data_name data)
add_definitions(-DPROTO_CONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/config/${proto_name}.prototxt")
add_definitions(-DPROTO_DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/config/${data_name}.prototxt")
set(src_lists src/main.cpp src/iva.cpp)
set(proto_buf_dir ${CMAKE_CURRENT_SOURCE_DIR}/proto/${proto_name}.proto)
set(proto_buf_target ${CMAKE_CURRENT_SOURCE_DIR}/proto/)
set(proto_buf_data ${CMAKE_CURRENT_SOURCE_DIR}/config)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O2")
set(OUT_DIR ${proto_buf_target})
find_package(Eigen3 REQUIRED)
find_package(Protobuf REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "proto file dirctory is "  ${PROTOBUF_INCLUDE_DIR})
message(STATUS "proto libs dirctory is "  ${PROTOBUF_LIBRARY})
modified_protobuf_generate_cpp(${proto_buf_target} PROTO_SRCS PROTO_HDRS ${proto_buf_dir}) 
add_executable(${PROJECT_NAME} ${src_lists} ${PROTO_SRCS} ${PROTO_HDRS})
message(STATUS "proto file " ${PROTO_SRCS} " has been generated ")
target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES})
set(lib_src_lists src/iva.cpp ${PROTO_SRCS} ${PROTO_HDRS})
add_library(${PROJECT_NAME}_lib SHARED ${lib_src_lists})
target_link_libraries(${PROJECT_NAME}_lib ${PROTOBUF_LIBRARIES})
INSTALL(TARGETS ${PROJECT_NAME}_lib 
		LIBRARY DESTINATION lib				
		)
install(FILES ${PROJECT_SOURCE_DIR}/src/iva.h DESTINATION include/iva)

