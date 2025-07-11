cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(COMPONENTS 
    micro_ros_espidf_component
    main
)

project(ecsbox)


