cmake_minimum_required(VERSION 3.20)
project(c_projects C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(projects/search)
add_subdirectory(projects/explore)
add_subdirectory(projects/write)

if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror)
  add_compile_options(-g)
  add_link_options(-g)
endif()

option(ENABLE_SANITIZERS "Enable ASan/UBSan" ON)
if (ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-fsanitize=address,undefined)
  add_link_options(-fsanitize=address,undefined)
endif()
