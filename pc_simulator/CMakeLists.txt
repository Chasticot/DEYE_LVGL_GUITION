cmake_minimum_required(VERSION 3.24)
project(deye_lvgl_pc_simulator LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(LV_CONF_INCLUDE_SIMPLE ON CACHE BOOL "" FORCE)
set(LV_CONF_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h" CACHE STRING "" FORCE)

FetchContent_Declare(lvgl
  GIT_REPOSITORY https://github.com/lvgl/lvgl.git
  GIT_TAG v8.4.0
  GIT_SHALLOW TRUE
)
FetchContent_Declare(lv_drivers
  GIT_REPOSITORY https://github.com/lvgl/lv_drivers.git
  GIT_TAG release/v8.3
  GIT_SHALLOW TRUE
)
FetchContent_Declare(sdl2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.30.10
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(lvgl sdl2)
# lv_drivers builds every hardware backend when added as a subproject.  We only
# need its SDL sources, so fetch them without adding their broken aggregate target.
FetchContent_GetProperties(lv_drivers)
if(NOT lv_drivers_POPULATED)
  FetchContent_Populate(lv_drivers)
endif()

add_library(lvgl_sdl_driver STATIC
  ${lv_drivers_SOURCE_DIR}/sdl/sdl.c
  ${lv_drivers_SOURCE_DIR}/sdl/sdl_common.c
)
target_include_directories(lvgl_sdl_driver PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${lv_drivers_SOURCE_DIR}
  ${lvgl_SOURCE_DIR}
)
target_compile_definitions(lvgl_sdl_driver PRIVATE LV_CONF_INCLUDE_SIMPLE LV_LVGL_H_INCLUDE_SIMPLE)
target_link_libraries(lvgl_sdl_driver PUBLIC lvgl SDL2::SDL2)

add_executable(deye_lvgl_pc_sim main.cpp)
target_include_directories(deye_lvgl_pc_sim PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/compat
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${lv_drivers_SOURCE_DIR}
  ${lvgl_SOURCE_DIR}
)
target_compile_definitions(deye_lvgl_pc_sim PRIVATE LV_CONF_INCLUDE_SIMPLE LV_LVGL_H_INCLUDE_SIMPLE)
target_link_libraries(deye_lvgl_pc_sim PRIVATE lvgl_sdl_driver lvgl SDL2::SDL2main SDL2::SDL2)

enable_testing()
add_executable(modbus_tcp_codec_test modbus_tcp_codec_test.cpp)
target_include_directories(modbus_tcp_codec_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
add_test(NAME modbus_tcp_codec_test COMMAND modbus_tcp_codec_test)
