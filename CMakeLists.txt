############################
###### Cmake template ######
############################

# author: sami dhiab
# email: sami@theion.de



cmake_minimum_required(VERSION 3.5.0)

# project name and version: change project name
project(<project_name> VERSION "1.0")

# set the C++ standard: 20 is set
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# includes
include_directories(${PROJECT_SOURCE_DIR}/includes/)
# dependencies manual includes: change repo name
include_directories(${PROJECT_SOURCE_DIR}/dependencies/<repo_name>/includes/)

# add all source *.cpp : change repo name
file(GLOB all_SRCS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
## manual dependencies includes
    "${PROJECT_SOURCE_DIR}/dependencies/<repo_name>/src/*.cpp"

    )

# make the exec: change project name
add_executable(<project_name> ${all_SRCS})

# link againt static lib
target_link_libraries(<project_name> ${PROJECT_SOURCE_DIR}/dependencies/<repo_name>/<static_name>.lib

)

# add child cmake
add_subdirectory(dependencies/<repo_name>)