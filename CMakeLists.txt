cmake_minimum_required(VERSION 3.8)
project(ISA_Master)

link_libraries(ssl)
link_libraries(crypto)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES main.cpp ParseParameters.cpp ParseParameters.h FileManipulator.cpp FileManipulator.h Connection.cpp Connection.h Error.cpp Error.h ConnectionInterface.h ConnectionInterface.cpp TLSConnection.cpp TLSConnection.h)
add_executable(ISA_Master ${SOURCE_FILES})
