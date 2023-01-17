############################
###### Cmake template ######
############################

# author: sami dhiab
# email: sami@theion.de

# only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.1.0)

# project name
project(ph_cooling VERSION "1.0")

# set the C++17 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
enable_testing()
#This is necessary for MSVC to create a symbol file, .lib, besides a shared library, .dll
if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
########################## add subdirectory##################

set(CMAKE_PREFIX_PATH "C:/vcpkg/installed/x64-windows/share")
find_package(sockpp CONFIG REQUIRED)

find_package(yaml-cpp CONFIG REQUIRED)

# add dependencies cmake
#add_subdirectory(dependencies/lib_keyence_sdk )

#add_subdirectory(dependencies/sockpp )
#if(NOT TARGET sockpp-objs ${SOCKPP} ${SOCKPP_STATIC})
#        add_subdirectory(dependencies/sockpp)
#        install(TARGETS ${SOCKPP_STATIC})  
#endif()

#if(NOT TARGET yaml-cpp)
#        add_subdirectory(dependencies/yaml-cpp )
#        install(TARGETS yaml-cpp)  
#endif()

add_subdirectory(dependencies/software_printhead_module)

add_definitions(-D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)  
add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)  

################## search and append ##########################
find_file(ph_config 
        NAMES ph_config.yaml
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/config
        REQUIRED
)

#find_file(delta_server 
#        NAMES delta_server.py
#        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/software_repetier_rest_api/src/repetier_manager_lib/
#        REQUIRED
#)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development )

message("found python interpreter: ${Python3_EXECUTABLE}")
message("found ph config file: ${ph_config}")        

#write_file(${PH_config} "interpreter: \"${Python3_EXECUTABLE}\"" APPEND)
#write_file(${PH_config} "script: \"${delta_server}\"" APPEND)
set ( PH_CONFIG_BUILD  "${ph_config}" CACHE FILEPATH "${ph_config}")
set ( PH_CONFIG  "${CMAKE_INSTALL_PREFIX}/config/ph_config.yaml" CACHE FILEPATH "${CMAKE_INSTALL_PREFIX}/config/ph_config.yaml")

set (PY_INTERPRETER "${Python3_EXECUTABLE}")

add_definitions(-DPY_INTERPRETER="${PY_INTERPRETER}") 
add_definitions(-DPH_CONFIG="${PH_CONFIG}") 

# copy config one by one 
function(copy_config target_exe config_file)
add_custom_command(TARGET ${target_exe} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${config_file} "${CMAKE_INSTALL_PREFIX}/config/" 
COMMENT "copy config from ${config_file} to ${CMAKE_INSTALL_PREFIX}/config/"
    )
endfunction()

###################### Build Library ############################

# source *.cpp
file(GLOB lib_SRCS
        "${PROJECT_SOURCE_DIR}/src/Iaxis_motion.cpp"
        "${PROJECT_SOURCE_DIR}/src/linear_motion.cpp"
        "${PROJECT_SOURCE_DIR}/src/rotation_motion.cpp"
        "${PROJECT_SOURCE_DIR}/src/ph_cooling_controller.cpp"
        # mocking files
        #"${PROJECT_SOURCE_DIR}/tests/sensorMock.cpp"
        #"${PROJECT_SOURCE_DIR}/tests/axisMock.cpp"

)
# add libraries
add_library(ph_cooling_static STATIC ${lib_SRCS})
# link againt static lib
target_link_libraries(ph_cooling_static 
# ph interface lib
PUBLIC ph_interface_lib
# sockpp lib
PUBLIC ${SOCKPP_LIBRARIES}
#yaml lib
PUBLIC yaml-cpp
# py
PRIVATE ${Python_LIBRARIES}
)
# include libs
target_include_directories(ph_cooling_static 
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tests/

PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/software_printhead_module/includes
PUBLIC ${SOCKPP_INCLUDE_DIRS}
PRIVATE ${Python_INCLUDE_DIRS}

)
target_link_directories(ph_cooling_static PRIVATE
        ${Python_LIBRARY_DIRS})

############################# install target lib ########################################
INSTALL(FILES ${PH_CONFIG_BUILD} DESTINATION config/)
message("config file PH copied from ${PH_CONFIG_BUILD} to: ${CMAKE_INSTALL_PREFIX}/config/")
set(PH_CONFIG "${CMAKE_INSTALL_PREFIX}/config/ph_config.yaml")
add_definitions(-DPH_CONFIG="${PH_CONFIG}") 
message("config file is copied to ${PH_CONFIG}")

install(TARGETS ph_cooling_static DESTINATION lib)
FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h" 
)
INSTALL(FILES ${files} DESTINATION include/ph_cooling_static/)

############################# build executable ###################################

## source *.cpp
file(GLOB exe_SRCS
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        # mocking files
        #"${PROJECT_SOURCE_DIR}/tests/sensorMock.cpp"
        #"${PROJECT_SOURCE_DIR}/tests/axisMock.cpp"
)
# add .exe
add_executable(ph_cooling_exe ${exe_SRCS})

target_link_libraries(ph_cooling_exe 
# ph interface lib
PUBLIC ph_interface_lib
# sockpp lib
PUBLIC ${SOCKPP_LIBRARIES}
#yaml lib
PUBLIC yaml-cpp
# py
PRIVATE ${Python_LIBRARIES}
)
# include libs
target_include_directories(ph_cooling_exe 
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tests/

PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/software_printhead_module/includes
PUBLIC ${SOCKPP_INCLUDE_DIRS}
PRIVATE ${Python_INCLUDE_DIRS}
)
target_link_directories(ph_cooling_exe PRIVATE
        ${Python_LIBRARY_DIRS})