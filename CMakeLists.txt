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
#set(CMAKE_PREFIX_PATH "${VCPKG_ROOT}/installed/x64-windows/share")
find_package(sockpp CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
add_subdirectory(dependencies/software_printhead_module)
add_definitions(-D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)  
add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)  
################## search and append ##########################
find_file(ph_config 
        NAMES ph_config.yaml
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/config
        REQUIRED
)
message("found ph config file: ${ph_config}")        
set (PH_CONFIG  "${ph_config}" CACHE FILEPATH "${ph_config}")
set (PH_CONFIG_DIR  ${CMAKE_INSTALL_PREFIX}/config/ph_config.yaml CACHE FILEPATH "${CMAKE_INSTALL_PREFIX}/config/ph_config.yaml}")
add_definitions(-DPH_CONFIG="${PH_CONFIG_DIR}") 

###################### Build Library ############################
# source *.cpp
file(GLOB lib_SRCS
        "${PROJECT_SOURCE_DIR}/src/Iph_axis_motion.cpp"
        "${PROJECT_SOURCE_DIR}/src/ph_linear_motion.cpp"
        "${PROJECT_SOURCE_DIR}/src/ph_rotation_motion.cpp"
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
PUBLIC wsock32 
PUBLIC ws2_32
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
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/software_printhead_module/includes/
PUBLIC ${SOCKPP_INCLUDE_DIRS}
PRIVATE ${Python_INCLUDE_DIRS}
)
target_link_directories(ph_cooling_static PRIVATE ${Python_LIBRARY_DIRS})
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
PUBLIC wsock32 
PUBLIC ws2_32
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

##################### Post Build ####################
# copy config one by one 
function(copy_ph_config target_exe config_file)
add_custom_command(TARGET ${target_exe} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy ${config_file} ${PH_CONFIG_DIR} 
COMMENT "copy config from ${config_file} to ${PH_CONFIG_DIR}"
    )
endfunction()
# copy dlls one by one 
function(copy_dlls_yaml_target_destination target_exe)
#add_custom_command(TARGET ${target_exe} POST_BUILD
#COMMAND ${CMAKE_COMMAND} -E copy_if_different  $<TARGET_FILE:yaml-cpp> $<TARGET_FILE_DIR:${target_exe}>
#COMMAND_EXPAND_LISTS)
endfunction()


copy_ph_config(ph_cooling_static ${PH_CONFIG})


copy_dlls_meteor_target_destination(ph_cooling_exe)
copy_ph_config(ph_cooling_exe ${PH_CONFIG})
copy_dlls_yaml_target_destination(ph_cooling_exe)

############################# install targets ########################################
        FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h" )
        INSTALL(FILES ${files} DESTINATION include/ph_cooling_static/)
        install(TARGETS ph_cooling_exe ph_cooling_static
        CONFIGURATIONS Debug
        RUNTIME DESTINATION bin/Debug
        LIBRARY DESTINATION lib)
