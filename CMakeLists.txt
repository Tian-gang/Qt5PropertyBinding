cmake_minimum_required(VERSION 3.14)

project(Qt5PropertyBinding)

include(GNUInstallDirs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

macro(source_group_by_dir source_files)       
	set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR}/)
	foreach(sgbd_file ${${source_files}})
		string(REGEX REPLACE ${sgbd_cur_dir} "" sgbd_fpath ${sgbd_file})
		string(REGEX REPLACE "[^/\\\\]+$" "" sgbd_group_name ${sgbd_fpath})
		if(sgbd_group_name)
			string(REPLACE "/"   "\\\\"  sgbd_group_name_normal ${sgbd_group_name})
			#message(STATUS ${sgbd_group_name_normal})
			source_group(${sgbd_group_name_normal} FILES ${sgbd_file})
		endif(sgbd_group_name)	
	endforeach(sgbd_file)
endmacro(source_group_by_dir)

add_subdirectory(demo)
add_subdirectory(src)


