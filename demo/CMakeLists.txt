cmake_minimum_required(VERSION 3.14)

project(Demo LANGUAGES CXX)


file(GLOB_RECURSE ALL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                              ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)


add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${ALL_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

target_compile_definitions(${PROJECT_NAME} PRIVATE QT_USE_QSTRINGBUILDER)
target_link_libraries(${PROJECT_NAME} PRIVATE PropertyBinding)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

add_dependencies(${PROJECT_NAME} PropertyBinding)

source_group_by_dir(ALL_SOURCES)
