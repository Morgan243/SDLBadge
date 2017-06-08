cmake_minimum_required(VERSION 3.6)
project(BasicSDLBadge)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c sdl_fb.c colors.h sdl_buttons.c sdl_buttons.h badgelandia.c utils.c screensaver.c groundwar.c vector_letters.c.cpp badge_menu.c boot_splash.c)

add_definitions(-DSDL_BADGE)
add_executable(BasicSDLBadge ${SOURCE_FILES})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lm")
find_package(SDL2)
#PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
target_link_libraries(BasicSDLBadge ${SDL2_LIBRARIES})
