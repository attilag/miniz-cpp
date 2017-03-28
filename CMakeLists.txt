cmake_minimum_required(VERSION 2.8)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.1")
  cmake_policy(SET CMP0054 NEW)
endif()

project(miniz-cpp)

if (CMAKE_COMPILER_IS_GNUCC OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  option(ENABLE_LTO "Enable Link Time Optimization" FALSE)
  if (ENABLE_LTO)
    add_definitions(-flto)
    set(LINKER_FLAGS "${LINKER_FLAGS} -flto")
  endif()
endif()

# build in the source directory
list(APPEND CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_BINARY_DIR}")
# ignore scm
list(APPEND CPACK_SOURCE_IGNORE_FILES "\\\\.git")
# ignore vim files
list(APPEND CPACK_SOURCE_IGNORE_FILES ".swp")
# ignore general editor files
list(APPEND CPACK_SOURCE_IGNORE_FILES ".*~")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)

set(MINIZCPP_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})

include(CPack)

if(CMAKE_COMPILER_IS_GNUCC)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)

  if(GCC_VERSION VERSION_LESS 4.9)
    set(CPP11_FLAG "-std=c++1y")
  else()
    set(CPP11_FLAG "-std=c++14")
  endif()
else()
  set(CPP11_FLAG "-std=c++14")
endif()

# fork from CHAISCRIPT
if(MSVC)
  add_definitions(/W4 /w14545 /w34242 /w34254 /w34287 /w44263 /w44265 /w44296 /w44311 /w44826 /we4289 /w14546 /w14547 /w14549 /w14555 /w14619 /w14905 /w14906 /w14928)

  if (MSVC_VERSION STREQUAL "1800")
    # VS2013 doesn't have magic statics
    add_definitions(/w44640)
  else()
    # enum warnings are too noisy on MSVC2013
    add_definitions(/w34062)
  endif()

  add_definitions(/bigobj)
  # Note on MSVC compiler flags.
  # The code base selective disables warnings as necessary when the compiler is complaining too much
  # about something that is perfectly valid, or there is simply no technical way around it
  # This particular warning, C4503 is in regards to the decorated names that MSVC generates internally.
  # The error did not come up until the move to C++11, but the compiler doesn't give enough information
  # to determine where the error is coming from, and the internet provides no real information for
  # how to workaround or fix the error. So I'm disabling it globally.
  add_definitions(/wd4503)
else()
  add_definitions(-Wall -Wextra -Wconversion -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wcast-qual -Wunused -Woverloaded-virtual  -pedantic ${CPP11_FLAG})

  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_definitions(-Weverything -Wno-c++98-compat-pedantic  -Wno-c++98-compat -Wno-documentation -Wno-switch-enum -Wno-weak-vtables -Wno-missing-prototypes -Wno-padded -Wno-missing-noreturn -Wno-exit-time-destructors -Wno-documentation-unknown-command)
  else()
    add_definitions(-Wnoexcept)
  endif()

  if(APPLE)
    add_definitions(-Wno-sign-compare)
  endif()
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  option(USE_LIBCXX "Use clang's libcxx" TRUE)

  if(USE_LIBCXX)
    add_definitions(-stdlib=libc++)
    set(LINKER_FLAGS "${LINKER_FLAGS} ${CPP11_FLAG} -stdlib=libc++")
  else()
    set(LINKER_FLAGS "${LINKER_FLAGS} ${CPP11_FLAG}")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC)
  set(LINKER_FLAGS "${LINKER_FLAGS} ${CPP11_FLAG}")
endif()

# limitations in MinGW require us to make an optimized build
# for the sake of object sizes or something
if(MINGW OR CYGWIN)
  add_definitions(-O3)
endif()

set (MinizCpp_INCLUDES miniz.h zip_file.hpp)
set_source_files_properties(${MinizCpp_INCLUDES} PROPERTIES HEADER_FILE_ONLY TRUE)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINKER_FLAGS}")

add_library(minizcpp miniz.c zip_file.cpp)
set(MINIZCPP_LIBS minizcpp)

add_executable(pipe pipe.cpp ${MinizCpp_INCLUDES})
add_executable(read read.cpp ${MinizCpp_INCLUDES})
add_executable(write write.cpp ${MinizCpp_INCLUDES})
target_link_libraries(pipe ${LIBS} ${MINIZCPP_LIBS})
target_link_libraries(read ${LIBS} ${MINIZCPP_LIBS})
target_link_libraries(write ${LIBS} ${MINIZCPP_LIBS})
