cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(SystemVerilog
    VERSION 0.2.0
    DESCRIPTION "SystemVerilog Language Interpretator Module")

SET(CMAKE_SHARED_MODULE_PREFIX_CXX "")
SET(CMAKE_SHARED_MODULE_SUFFIX_CXX ".simodo-semantics")

file(GLOB_RECURSE CPPS  ./*.cpp )
file(GLOB_RECURSE HPPS  ./*.hpp ./*.h )

add_library(${PROJECT_NAME} MODULE
    ${CPPS} ${HPPS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
#    SOVERSION 1
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS YES
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src/include)

target_link_libraries(${PROJECT_NAME} SIMODO-interpret)

set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/semantics)
