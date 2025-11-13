cmake_minimum_required(VERSION 3.30)

# The toolchain file setup should ideally be in a CMakePresets.json file,
# but we'll keep it here for simplicity
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

project(NorthernKingdom VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add dependencies
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(gli CONFIG REQUIRED)
#find_package(GLEW CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

# No C++20 modules
set(CMAKE_CXX_SCAN_FOR_MODULES OFF) 

# Playground executable
add_executable(NorthernKingdom)

# Playground sources and headers
file(GLOB_RECURSE NorthernKingdom_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Shader/*.*"
)

target_sources(NorthernKingdom PRIVATE ${NorthernKingdom_SOURCES})

# Playground include dirs
target_include_directories(NorthernKingdom PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils.h
)

# Preprocessor defines
target_compile_definitions(NorthernKingdom PRIVATE
    MY_TEST_DEFINE
)

# Automatically links to these libs
target_link_libraries(NorthernKingdom PRIVATE
    glfw
    GLEW::GLEW
    glm::glm
    gli
    assimp::assimp
)

# Add directories with any random loose .dlls or .libs here
target_link_directories(NorthernKingdom PRIVATE
  
)

# Set configuration properties for Playground
if(MSVC)
    target_compile_options(NorthernKingdom PRIVATE
        /W3
        #/MP # multithreaded build
        #/WX # warnings as errors
        #/ZI # program database for edit and continue
    )
else()
    target_compile_options(NorthernKingdom PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
    )
endif()

# Set Playground as the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT NorthernKingdom)

# Filters
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${NorthernKingdom_SOURCES})

# Enable solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set_target_properties(NorthernKingdom PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
