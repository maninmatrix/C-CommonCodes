SET(ProjectName TestXml)
PROJECT(${ProjectName})
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
INCLUDE_DIRECTORIES( ../)
INCLUDE_DIRECTORIES( ../include)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)

add_definitions(-std=c++11)

SET(LINK_LIBPATH "../TinyXml/lib")
#target_link_libraries(${ProjectName} ${LINK_LIBPATH})
LINK_DIRECTORIES(${LINK_LIBPATH})

#ADD_LIBRARY(${ProjectName}  ${DIR_SRCS})
add_executable(${ProjectName} ${DIR_SRCS})

target_link_libraries(${ProjectName}  libTinyXml.a)
#add_subdirectory("./" "../build")
set(EXECUTABLE_OUTPUT_PATH "../bin")PUT_PATH "../bin")