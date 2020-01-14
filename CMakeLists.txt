set(servicex_src_files
  main.cpp
  Client.cpp
)

set(servicex_header_files
  Client.h
)

cmake_minimum_required(VERSION 2.8)
project(httpservicex)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Boost 1.60.0 REQUIRED COMPONENTS thread system date_time filesystem)

include_directories(${Boost_INCLUDE_DIR})

add_executable(httpservicex ${servicex_src_files} ${servicex_header_files})

target_link_libraries(httpservicex ${Boost_LIBRARIES} lz4 snappy)

install(TARGETS httpservicex RUNTIME DESTINATION bin)
