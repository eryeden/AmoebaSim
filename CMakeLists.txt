PROJECT(MCsim_phase1)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#For auto-complete
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CMAKE_VERBOSE_MAKEFILE ON)

#find_package(PkgConfig REQUIRED)
#pkg_search_module(GLFW REQUIRED glfw3)


#Find packageでライブラリの設定をすべきか？
#それとも、なかにぶち込んでるライブラリを使うべきか？

#とりあえずFind packageしてみる
#よくわからなかった

#Libraries configurations
#OpenGL
SET(GL_INCLUDE ${PROJECT_SOURCE_DIR}/OpenGL/include)
SET(GL_LIB ${PROJECT_SOURCE_DIR}/OpenGL/lib/linux)
SET(GL_SRC_PREFIX ${PROJECT_SOURCE_DIR}/OpenGL/src)
#ディレクトリからソースコードを追加
file(GLOB  GL_SRCS ${GL_SRC_PREFIX}/*.cpp)
#ライブラリ名の追加
#SET(GL_LIBS GL ${GLFW_STATIC_LIBRARIES} assimp pthread)
SET(GL_LIBS GL glfw3 rt m dl Xrandr Xinerama Xxf86vm Xcursor Xext Xrender Xfixes X11 pthread xcb Xau Xdmcp assimp)


#Common libraries
SET(COMMON_INCLUDE ${PROJECT_SOURCE_DIR}/Common)

#Simulator
SET(SIM_INCLUDE ${PROJECT_SOURCE_DIR}/include)
SET(SIM_SRC_PREFIX ${PROJECT_SOURCE_DIR}/src)
file(GLOB SIM_SRCS ${SIM_SRC_PREFIX}/*/*.cpp)

INCLUDE_DIRECTORIES(
  ${GL_INCLUDE}
  ${COMMON_INCLUDE}
  ${SIM_INCLUDE}
  )
set(SRCS
  ${GL_SRCS}
  ${SIM_SRCS}
  )

link_directories(
  ${GL_LIB}
  )

#Compile options
SET(COMPILEOPTIONS "-g -Wall")
SET(COMPILEOPTIONSXX "-std=c++11")
SET(LINKOPTIONS "-g")


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILEOPTIONS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} ${COMPILEOPTIONSXX}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKOPTIONS}")

message("#################")
message("${SIM_SRCS}")

message("#################")
message("${GL_SRCS}")

message("#################")
message("${GL_LIBS}")

message("#################")
message("${GL_INCLUDE};${COMMON_INCLUDE};${SIM_INCLUDE}")

message("##C_FLAGS##")
message(${CMAKE_C_FLAGS})
message("##CXX_FLAGS##")
message(${CMAKE_CXX_FLAGS})

message("##LD_FLAGS##")
message(${CMAKE_EXE_LINKER_FLAGS})

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


add_executable(${CMAKE_PROJECT_NAME} ${SRCS})

target_link_libraries(${CMAKE_PROJECT_NAME} ${GL_LIBS})

set(TARGET ${CMAKE_PROJECT_NAME})

#ALLを入れてないと自動実行
#入れいていない時は
#make ${CMAKE_PROJECT_NAME}.hex
#のようにコマンドを打てば実行可能
# ADD_CUSTOM_TARGET(${TARGET}.hex
#   ALL
#   DEPENDS ${TARGET}
#   COMMAND ${CMAKE_OBJCOPY} -Oihex ${CMAKE_BINARY_DIR}/bin/${TARGET} ${CMAKE_BINARY_DIR}/bin/${TARGET}.hex)
# ADD_CUSTOM_TARGET(${TARGET}.bin
#   ALL
#   DEPENDS ${TARGET}
#   COMMAND ${CMAKE_OBJCOPY} -Obinary ${CMAKE_BINARY_DIR}/bin/${TARGET} ${CMAKE_BINARY_DIR}/bin/${TARGET}.bin)


SET(COMPILE_COMMANDS "compile_commands.json")
ADD_CUSTOM_TARGET(${COMPILE_COMMANDS}
  ALL
  DEPENDS ${COMPILE_COMMANDS}
  COMMAND cp ${COMPILE_COMMANDS} ${PROJECT_SOURCE_DIR}/${COMPILE_COMMANDS})





























































