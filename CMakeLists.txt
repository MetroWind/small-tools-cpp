cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
project(NSShot)

add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)

include(FetchContent)
FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG v3.1.1
)

FetchContent_MakeAvailable(cxxopts)

set(SOURCE_FILES main.cpp)
add_executable(nsshot ${SOURCE_FILES})
set_property(TARGET nsshot PROPERTY CXX_STANDARD 20)

include(CMakePrintHelpers)
cmake_print_variables(ImageMagick_INCLUDE_DIRS)

target_include_directories(nsshot PRIVATE
  ${ImageMagick_INCLUDE_DIRS}
  ${cxxopts_SOURCE_DIR}/include
)
target_link_libraries(nsshot ${ImageMagick_LIBRARIES})
