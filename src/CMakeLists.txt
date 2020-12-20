cmake_minimum_required(VERSION 3.14)

project(PropertyBinding LANGUAGES CXX)


set(CMAKE_INCLUDE_CURRENT_DIR ON)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

file(GLOB_RECURSE ALL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                              ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} PRIVATE ${ALL_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(${PROJECT_NAME} PRIVATE QT_USE_QSTRINGBUILDER)
target_compile_definitions(${PROJECT_NAME} PRIVATE QPROPERTY_BINDING_LIBRARY)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

source_group_by_dir(ALL_SOURCES)
