cmake_minimum_required(VERSION 3.8)
project(ISA_Master)

link_libraries(ssl)
link_libraries(crypto)
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES src/main.cpp src/ParseParameters.cpp src/ParseParameters.h src/FileManipulator.cpp src/FileManipulator.h src/Connection.cpp src/Connection.h src/Error.cpp src/Error.h src/ConnectionInterface.h src/ConnectionInterface.cpp src/TLSConnection.cpp src/TLSConnection.h)
add_executable(ISA_Master ${SOURCE_FILES})
