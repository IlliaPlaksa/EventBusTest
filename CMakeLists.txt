cmake_minimum_required(VERSION 3.21)
project(Test)

set(CMAKE_CXX_STANDARD 17)

set(PROTO_DIR ${PROJECT_SOURCE_DIR}/proto)

file(GLOB PROTO_SOURCES CONFIGURE_DEPENDS ${PROTO_DIR}/*.proto)
message("Proto srcs ${PROTO_SOURCES}")
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(gRPC REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

include_directories(${GENERATED_DIR})

find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin REQUIRED)

macro(compile_proto_file filename output_dir base_dir)
    get_filename_component(dirname ${filename} DIRECTORY)
    get_filename_component(basename ${filename} NAME_WE)

    if("${dirname}" STREQUAL "")
        set(OUTPUT_FILE_PB "${output_dir}/${basename}.pb.cc")
        set(OUTPUT_FILE_GRPC "${output_dir}/${basename}.grpc.pb.cc")
    else()
        set(OUTPUT_FILE_PB "${output_dir}/${dirname}/${basename}.pb.cc")
        set(OUTPUT_FILE_GRPC "${output_dir}/${dirname}/${basename}.grpc.pb.cc")
    endif()
    add_custom_command(
            OUTPUT "${OUTPUT_FILE_PB}" "${OUTPUT_FILE_GRPC}"
            COMMAND ${Protobuf_PROTOC_EXECUTABLE} ${base_dir}/${filename}
            --cpp_out=${output_dir}
            --grpc_out=${output_dir}
            --plugin=protoc-gen-grpc=${_GRPC_CPP_PLUGIN_EXECUTABLE}
            -I ${base_dir}
            DEPENDS ${base_dir}/${filename}
            VERBATIM
    )
    list(APPEND proto_generated_files "${OUTPUT_FILE_PB}")
    list(APPEND proto_generated_files "${OUTPUT_FILE_GRPC}")
endmacro(compile_proto_file)

macro(compile_proto_dir generated_cpp_dir file_list base_dir)
    foreach(proto_file ${file_list})
        file(RELATIVE_PATH relative_file_path ${base_dir} ${proto_file})
        message("working on ${proto_file}")
        compile_proto_file(${relative_file_path} ${generated_cpp_dir} ${base_dir})
    endforeach(proto_file)
endmacro(compile_proto_dir)

message("Generated ${GENERATED_DIR}")
message("Proto srcs ${PROTO_SOURCES}")
message("Proto dir ${PROTO_DIR}")
compile_proto_dir(${GENERATED_DIR} "${PROTO_SOURCES}" "${PROTO_DIR}")

SET_SOURCE_FILES_PROPERTIES(${proto_generated_files} PROPERTIES GENERATED TRUE)

# eventpp library
find_package(eventpp CONFIG REQUIRED)
set(EVENTPP_INCLUDE_DIR ${eventpp_DIR}/include)
include_directories(${EVENTPP_INCLUDE_DIR})

# Dexode::EventBus library
include_directories(${EVENT_BUS_ROOT}/include)
add_subdirectory(${EVENT_BUS_ROOT}/lib ${EVENT_BUS_ROOT}/lib/build)

file(GLOB SRCS src/*.h src/*.cpp)

add_executable(Test
        ${SRCS}
        ${PROTO_SRCS} 
        ${PROTO_HDRS} 
        ${proto_generated_files})
target_include_directories(Test 
        PUBLIC 
        ${Boost_INCLUDE_DIR}/include
        ${EVENTPP_INCLUDE_DIR}
        )

target_link_libraries(Test LINK_PUBLIC ${Boost_LIBRARIES} ${Protobuf_LIBRARIES} eventpp::eventpp)
target_link_libraries(Test PRIVATE eventpp::eventpp)
target_link_libraries(Test PUBLIC Dexode::EventBus)
