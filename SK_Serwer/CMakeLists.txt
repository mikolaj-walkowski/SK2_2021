cmake_minimum_required(VERSION 3.10)

# set the project name
project(SK_Serwer)

# add the executable
include_directories(${PROJECT_SOURCE_DIR}/headers)

add_executable(${PROJECT_NAME} server.cpp Client.cpp Room.cpp enums.cpp)