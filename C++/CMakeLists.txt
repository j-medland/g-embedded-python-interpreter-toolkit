cmake_minimum_required(VERSION 3.21)
include(GenerateExportHeader)

project(gepit VERSION 1.0.0)

SET(INSTALL_DIR ${CMAKE_CURRENT_LIST_DIR}/../LabVIEW)

## Determine the bitness
math(EXPR BITS "8 * ${CMAKE_SIZEOF_VOID_P}")

## build pybind11
add_subdirectory(pybind11)

## find labVIEW wrapper dependencies
# pybind11 targets are built locally and exported into the pybind11 namespace
# pybind11 also performs the Python3.x search and using pybind11::embed 
# adds headers and Python lib to target-link-dependencies

## create shared library
add_library(${PROJECT_NAME} SHARED)

# header files (relative to include/gepit)
set(HEADER_FILES 
    lv-interop.hpp
    gepit.hpp
)

# prepend include path
list(TRANSFORM HEADER_FILES PREPEND ${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}/)

# define target_sources
target_sources(${PROJECT_NAME}
    PRIVATE
    src/call-function.cpp
    src/eval.cpp
    src/exec.cpp
    src/interpreter.cpp
    src/lv-interop.cpp
    src/py-object.cpp
    src/session.cpp
    src/util.cpp
    ${HEADER_FILES}
)

# set single-value target properties
set_target_properties(${PROJECT_NAME} PROPERTIES 
    OUTPUT_NAME ${PROJECT_NAME}.cp${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}.${BITS}
    CXX_STANDARD 20
)
# set list based target properties
set_property(TARGET ${PROJECT_NAME} PROPERTY PUBLIC_HEADER ${HEADER_FILES})

# auto-generate the export header
generate_export_header(${PROJECT_NAME})

# define env bitness used for lv-interop.hpp
target_compile_definitions(${PROJECT_NAME} PRIVATE _${BITS}_BIT_ENV_)

# add the dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::embed)

# add the include dirs
target_include_directories(${PROJECT_NAME} 
    PUBLIC
    include
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # add the build directory to include the auto-generated export header
)

# define the install locations for the binaries (pybind11 stuff builds as LIBRARY targets)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${INSTALL_DIR}/bin
    LIBRARY DESTINATION ${INSTALL_DIR}/bin
    PUBLIC_HEADER DESTINATION ${INSTALL_DIR}/include/${PROJECT_NAME} # headers not really used by LabVIEW
)

# install the export header
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_export.h DESTINATION ${INSTALL_DIR}/include)