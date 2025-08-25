cmake_minimum_required(VERSION 3.9)
project(opcua)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(source_supervisor_dir ${CMAKE_CURRENT_SOURCE_DIR}/src_supervisor)
set(source_node_dir ${CMAKE_CURRENT_SOURCE_DIR}/src_node)
set(header_ext .h)
set(source_ext .cpp)

# Targets
set(target_supervisor supervisor)
set(target_node node)

# Supervisor target
file(GLOB_RECURSE sources_supervisor ${source_supervisor_dir}/*${source_ext})
file(GLOB_RECURSE headers_supervisor ${source_supervisor_dir}/*${header_ext})
add_executable(${target_supervisor} supervisor${source_ext} ${sources_supervisor})
target_link_libraries(${target_supervisor} m open62541 pthread)

# Node target
file(GLOB_RECURSE sources_node ${source_node_dir}/*${source_ext})
file(GLOB_RECURSE headers_node ${source_node_dir}/*${header_ext})
add_executable(${target_node} node${source_ext} ${sources_node})
target_link_libraries(${target_node} m open62541 pthread)


# Set headers
foreach(_header ${headers_supervisor})
    get_filename_component(_include_dir ${_header} DIRECTORY)
    target_include_directories(${target_supervisor} PRIVATE ${_include_dir})
endforeach()

foreach(_header ${headers_node})
    get_filename_component(_include_dir ${_header} DIRECTORY)
    target_include_directories(${target_node} PRIVATE ${_include_dir})
endforeach()
