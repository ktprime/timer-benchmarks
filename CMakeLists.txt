cmake_minimum_required(VERSION 3.0)

project(TimerBenchmark)

set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(THIRDPARTY_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd)

include_directories(
    src
)


if (CMAKE_BUILD_TYPE)
else()
    set(CMAKE_BUILD_TYPE RELEASE)
endif()


message("------------ Options -------------")
message("  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("  CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message("  CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR} ")
message("  CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message("  CMAKE_COMMAND: ${CMAKE_COMMAND}")
message("  CMAKE_OSX_ARCHITECTURES: ${CMAKE_OSX_ARCHITECTURES}")

if (WIN32)
    add_definitions(
        -DNOMINMAX
        -DWIN32_LEAN_AND_MEAN
        -D_CRT_SECURE_NO_WARNINGS
        -D_SCL_SECURE_NO_WARNINGS
        -D_WINSOCK_DEPRECATED_NO_WARNINGS
    )

  set(CMAKE_CXX_FLAGS "/WX- /MP")
  set(CMAKE_C_FLAGS   "/WX- /MP")
  set(CMAKE_C_FLAGS_DEBUG   "/W1 /Zi /Od ${CMAKE_C_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "/W1 /Zi /Od ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_RELEASE "/Ob1 /Ot /Oi /Oy /GL ${CMAKE_C_FLAGS}")
  set(CMAKE_C_FLAGS_RELEASE   "/Ob1 /Ot /Oi /Oy /GL ${CMAKE_CXX_FLAGS}")

  if (${CMAKE_BUILD_TYPE} MATCHES "RELEASE")
    add_compile_options(/Ob2 /O2 /Ot /Oi /Oy /GL /utf-8 /MT)
  else()
    add_compile_options(/utf-8 /MTd)
  endif()
else()
  set(CMAKE_C_FLAGS     "${CMAKE_C_FLAGS} -march=native")
  set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -std=c++14 -march=native")
  set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 ${CMAKE_CXX_FLAGS}")
  set(CMAKE_C_FLAGS_RELEASE   "-DNDEBUG -O3 ${CMAKE_C_FLAGS}")
endif()


file(GLOB_RECURSE LIB_HEADER_FILES src/*.h)
file(GLOB_RECURSE LIB_SOURCE_FILES src/*.cpp)


macro(SOURCE_GROUP_BY_DIR source_files)
    set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
    foreach(sgbd_file ${${source_files}})
        string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
        string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
        string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
        string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
        if(sgbd_nogroup)
            set(sgbd_group_name "\\")
        endif(sgbd_nogroup)
        source_group(${sgbd_group_name} FILES ${sgbd_file})
    endforeach(sgbd_file)
endmacro(SOURCE_GROUP_BY_DIR)

SOURCE_GROUP_BY_DIR(LIB_HEADER_FILES)
SOURCE_GROUP_BY_DIR(LIB_SOURCE_FILES)

add_library(timers ${LIB_HEADER_FILES} ${LIB_SOURCE_FILES})

add_subdirectory(test)
