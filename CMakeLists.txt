cmake_minimum_required (VERSION 3.8...4.1.0)

set(CMAKE_CXX_COMPILER "g++")

project(WorldMaker VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Para que no se traten las warnings como errores
add_compile_options(-Wno-error=array-bounds)

include(FetchContent)
include(FindOpenGL)

# -----------------------------------------------------------------------------------------------------
#######################################################################################################
#######################################################################################################
# -----------------------------------------------------------------------------------------------------

# Vendor

set(VENDOR_PATH "${CMAKE_SOURCE_DIR}/vendor")

add_library(Vendor INTERFACE)

target_include_directories(Vendor INTERFACE
        ${CMAKE_SOURCE_DIR}/vendor/GLM
        ${CMAKE_SOURCE_DIR}/vendor/stb_image
        #   ${CMAKE_SOURCE_DIR}/vendor/IMGUI
)

target_compile_definitions(Vendor INTERFACE STB_IMAGE_IMPLEMENTATION)
target_compile_definitions(Vendor INTERFACE GLEW_STATIC)



# -----------------------------------------------------------------------------------------------------

# GLEW

FetchContent_Populate(glew URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip)
add_library(glew_s STATIC ${glew_SOURCE_DIR}/src/glew.c ${glew_SOURCE_DIR}/src/glewinfo.c)
target_include_directories(glew_s PUBLIC ${glew_SOURCE_DIR}/include)
target_compile_definitions(glew_s PRIVATE GLEW_BUILD PUBLIC GLEW_STATIC)
target_link_libraries(glew_s PUBLIC OpenGL::GL)

# -----------------------------------------------------------------------------------------------------

# GLFW

set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8)
FetchContent_MakeAvailable(glfw)

set(BUILD_SHARED_LIBS ON)

# -----------------------------------------------------------------------------------------------------

# GLM

FetchContent_Populate(
        glm
        URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${glm_SOURCE_DIR})
add_library(glm::glm ALIAS glm)

# -----------------------------------------------------------------------------------------------------

# IMGUI

FetchContent_Declare(
        imgui
        GIT_REPOSITORY  https://github.com/ocornut/imgui.git
        GIT_TAG         v1.92.3-docking
)
FetchContent_MakeAvailable(imgui)

message("${imgui_SOURCE_DIR}")

# In order to use a library which has cpp and h files, you need to create a library (add_library) and
# give it a source path (with the cpp files).
file (GLOB IMGUI_SRC "${imgui_SOURCE_DIR}/*.cpp" "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp" "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp")
add_library(ImGui STATIC ${IMGUI_SRC})
# In this case, the library has #include <AnotherLibrary> in its cpp files, like GLFW or GLEW, so
# we need to indicate where those glfw.h and glew.h files are.
target_link_libraries(ImGui PUBLIC glfw glew_s)
target_include_directories(ImGui PUBLIC "${imgui_SOURCE_DIR}")
# Lastly, add this new library to the function target_link_directories and also provice the path to
# the h files to the target_include_directories function


# -----------------------------------------------------------------------------------------------------

# Math

file (GLOB MATH_SRC "${CMAKE_SOURCE_DIR}/core/math/*.cpp")
add_library(Math STATIC ${MATH_SRC})
target_link_libraries(Math PUBLIC glm)
target_include_directories(Math PUBLIC ${CMAKE_SOURCE_DIR}/core/math)


# -----------------------------------------------------------------------------------------------------

# PRNG

# Vendor

set(VENDOR_PATH "${CMAKE_SOURCE_DIR}/vendor")

add_library(Vendor INTERFACE)

target_include_directories(Vendor INTERFACE
	${CMAKE_SOURCE_DIR}/vendor/GLM
	${CMAKE_SOURCE_DIR}/vendor/stb_image
#   ${CMAKE_SOURCE_DIR}/vendor/IMGUI
)

target_compile_definitions(Vendor INTERFACE STB_IMAGE_IMPLEMENTATION)
target_compile_definitions(Vendor INTERFACE GLEW_STATIC)

# -----------------------------------------------------------------------------------------------------

# ImGui

file (GLOB STB_IMAGE_SRC "${CMAKE_SOURCE_DIR}/vendor/stb_image/*.cpp")
add_library(STB_Image STATIC ${STB_IMAGE_SRC})
# target_link_libraries(STB_Image PUBLIC )
target_include_directories(STB_Image PUBLIC ${CMAKE_SOURCE_DIR}/vendor/stb_image)

# -----------------------------------------------------------------------------------------------------

# Utils
#
file (GLOB UTILS_SRC "${CMAKE_SOURCE_DIR}/core/utils/*.cpp")
add_library(Utils STATIC ${UTILS_SRC})
# target_link_libraries(STB_Image PUBLIC )
target_include_directories(Utils PUBLIC ${CMAKE_SOURCE_DIR}/core/utils)
target_compile_definitions(Utils PUBLIC PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")

# -----------------------------------------------------------------------------------------------------

# GLEW

FetchContent_Populate(glew URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip)
add_library(glew_s STATIC ${glew_SOURCE_DIR}/src/glew.c ${glew_SOURCE_DIR}/src/glewinfo.c)
target_include_directories(glew_s PUBLIC ${glew_SOURCE_DIR}/include)
target_compile_definitions(glew_s PRIVATE GLEW_BUILD PUBLIC GLEW_STATIC)
target_link_libraries(glew_s PUBLIC OpenGL::GL)

# -----------------------------------------------------------------------------------------------------

# GLFW

set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
FetchContent_Declare(
		glfw
		GIT_REPOSITORY https://github.com/glfw/glfw.git
		GIT_TAG 3.3.8)
FetchContent_MakeAvailable(glfw)

set(BUILD_SHARED_LIBS ON)

# -----------------------------------------------------------------------------------------------------

# GLM

FetchContent_Populate(
		glm
		URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${glm_SOURCE_DIR})
add_library(glm::glm ALIAS glm)

# -----------------------------------------------------------------------------------------------------

# IMGUI

FetchContent_Declare(
				imgui
				GIT_REPOSITORY  https://github.com/ocornut/imgui.git
				GIT_TAG         v1.92.3-docking
)
FetchContent_MakeAvailable(imgui)

message("${imgui_SOURCE_DIR}")

# In order to use a library which has cpp and h files, you need to create a library (add_library) and
# give it a source path (with the cpp files).
file (GLOB IMGUI_SRC "${imgui_SOURCE_DIR}/*.cpp" "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp" "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp")
add_library(ImGui STATIC ${IMGUI_SRC})
# In this case, the library has #include <AnotherLibrary> in its cpp files, like GLFW or GLEW, so
# we need to indicate where those glfw.h and glew.h files are.
target_link_libraries(ImGui PUBLIC glfw glew_s)
target_include_directories(ImGui PUBLIC "${imgui_SOURCE_DIR}")
# Lastly, add this new library to the function target_link_directories and also provice the path to
# the h files to the target_include_directories function

# -----------------------------------------------------------------------------------------------------

# Rendering

file (GLOB RENDERING_SRC "${CMAKE_SOURCE_DIR}/core/rendering/*.cpp")
add_library(Rendering STATIC ${RENDERING_SRC})
target_link_libraries(Rendering PUBLIC Vendor glm Math Utils glfw glew_s Input Time)
target_include_directories(Rendering PUBLIC ${CMAKE_SOURCE_DIR}/core/rendering)

# -----------------------------------------------------------------------------------------------------

# Math

file (GLOB MATH_SRC "${CMAKE_SOURCE_DIR}/core/math/*.cpp")
add_library(Math STATIC ${MATH_SRC})
target_link_libraries(Math PUBLIC glm)
target_include_directories(Math PUBLIC ${CMAKE_SOURCE_DIR}/core/math)

# -----------------------------------------------------------------------------------------------------

# Input

file (GLOB INPUT_SRC "${CMAKE_SOURCE_DIR}/core/input/*.cpp")
add_library(Input STATIC ${INPUT_SRC})
target_link_libraries(Input PUBLIC glfw glew_s Math Events)
target_include_directories(Input PUBLIC ${CMAKE_SOURCE_DIR}/core/input)

# -----------------------------------------------------------------------------------------------------

# Events

file (GLOB EVENTS_SRC "${CMAKE_SOURCE_DIR}/core/events/*.cpp")
add_library(Events STATIC ${EVENTS_SRC})
target_link_libraries(Events PUBLIC Input Math)
target_include_directories(Events PUBLIC ${CMAKE_SOURCE_DIR}/core/events)

# -----------------------------------------------------------------------------------------------------

# Time

file (GLOB TIME_SRC "${CMAKE_SOURCE_DIR}/core/time/*.cpp")
add_library(Time STATIC ${TIME_SRC})
target_link_libraries(Time PUBLIC glfw glew_s)
target_include_directories(Time PUBLIC ${CMAKE_SOURCE_DIR}/core/time)

# -----------------------------------------------------------------------------------------------------

# PRNG

file (GLOB PRNG_SRC "${CMAKE_SOURCE_DIR}/core/PRNG/*.cpp")
add_library(PRNG STATIC ${PRNG_SRC})
target_compile_definitions(PRNG PUBLIC PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")
target_include_directories(PRNG INTERFACE ${CMAKE_SOURCE_DIR}/core/PRNG)
target_link_libraries(PRNG PUBLIC Math)

# -----------------------------------------------------------------------------------------------------

# PerlinNoise

file (GLOB PERLINNOISE_SRC "${CMAKE_SOURCE_DIR}/core/perlinNoise/*.cpp")
add_library(PerlinNoise STATIC ${PERLINNOISE_SRC})
target_compile_definitions(PerlinNoise PUBLIC PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")
target_include_directories(PerlinNoise INTERFACE ${CMAKE_SOURCE_DIR}/core/perlinNoise)
target_link_libraries(PerlinNoise PUBLIC Math PRNG Utilities)

# -----------------------------------------------------------------------------------------------------

# Utilities

file (GLOB UTILITIES_SRC "${CMAKE_SOURCE_DIR}/core/utilities/*.cpp")
add_library(Utilities STATIC ${UTILITIES_SRC})
target_compile_definitions(Utilities PUBLIC PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")
target_include_directories(Utilities INTERFACE ${CMAKE_SOURCE_DIR}/core/utilities)
target_link_libraries(Utilities PUBLIC Math)

# -----------------------------------------------------------------------------------------------------
#######################################################################################################
#######################################################################################################
# -----------------------------------------------------------------------------------------------------

# Core

add_library(Core INTERFACE)
target_link_libraries(Core INTERFACE Rendering Vendor Math Input Events Time PRNG Utilities PerlinNoise)

# -----------------------------------------------------------------------------------------------------
#######################################################################################################
#######################################################################################################
# -----------------------------------------------------------------------------------------------------

add_executable(WorldMaker WorldMaker.cpp)

target_link_libraries(WorldMaker PUBLIC Core)