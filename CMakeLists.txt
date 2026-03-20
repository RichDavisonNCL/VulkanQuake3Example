cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(VulkanQuake3Example)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(Vulkan REQUIRED)

set(ASSET_ROOT "${CMAKE_SOURCE_DIR}/Assets/" CACHE STRING "" FORCE)
add_compile_definitions(ASSETROOTLOCATION="${ASSET_ROOT}") 

if(MSVC)
    add_compile_definitions("VK_USE_PLATFORM_WIN32_KHR") 
    add_compile_definitions("NOMINMAX")
    add_compile_definitions("WIN32_LEAN_AND_MEAN")  
endif()

################################################################################
# Global options
################################################################################
# Set this variable to override the default DXC location.
# set(DXC_CUSTOM_PATH "")

# Dear ImGUI support
set(COMPILE_WITH_IMGUI OFF CACHE BOOL "Compiles with Dear ImGUI. Only used internally for development.")
if(COMPILE_WITH_IMGUI)
    # Obtain ImGUI if this was not previously set up
    include(FetchContent)
    set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/External/imgui)
    if(NOT EXISTS ${IMGUI_DIR})
        FetchContent_Populate(
            imgui
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            SOURCE_DIR     ${IMGUI_DIR}
        )
    endif()

    add_compile_definitions("USE_IMGUI") 
endif()

################################################################################
# Sub-projects
################################################################################
add_subdirectory(NCLCoreClasses)
add_subdirectory(idTechLoaders)
add_subdirectory(VulkanRendering)

add_subdirectory(VulkanQuake3Example)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT VulkanQuake3Example)
endif()