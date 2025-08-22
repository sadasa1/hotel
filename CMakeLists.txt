cmake_minimum_required(VERSION 3.20)
project(HotelSurveillance LANGUAGES CXX)

# ---------- C++ Standard ----------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------- SFML 3 ----------
# If CMake can't find SFML automatically, point SFML_DIR at:
#   -DSFML_DIR="$(brew --prefix sfml)/lib/cmake/SFML"
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio)

# ---------- Includes ----------
include_directories(${CMAKE_SOURCE_DIR}/include)

# ---------- Sources ----------
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)



add_executable(hotel_game ${SOURCES})
target_link_libraries(hotel_game PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)

# ---------- Build Type Defaults ----------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type: Debug or Release" FORCE)
endif()

# ---------- Debug Config ----------
target_compile_options(hotel_game PRIVATE
    $<$<CONFIG:Debug>:-O0 -g -fno-omit-frame-pointer -fsanitize=address,undefined>
)
target_link_options(hotel_game PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

# ---------- Release Config ----------
target_compile_options(hotel_game PRIVATE
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)
