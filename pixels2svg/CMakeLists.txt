cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_COMPILE_WARNING_AS_ERROR TRUE)

project(Pixels2SVG)

find_package(PNG REQUIRED)

include(FetchContent)
FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG v3.1.1
)

FetchContent_MakeAvailable(cxxopts)

set(SOURCE_FILES main.cpp png.cpp)
add_executable(pixels2svg ${SOURCE_FILES})
set_property(TARGET pixels2svg PROPERTY CXX_EXTENSIONS FALSE)
set_property(TARGET pixels2svg PROPERTY CXX_STANDARD 23)

target_include_directories(pixels2svg PRIVATE
  ${PNG_INCLUDE_DIR}
  ${cxxopts_SOURCE_DIR}/include
)
target_link_libraries(pixels2svg ${PNG_LIBRARY})
