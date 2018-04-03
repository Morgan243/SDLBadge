cmake_minimum_required(VERSION 3.6)
project(BasicSDLBadge)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES
        main.c sdl_fb.c sdl_buttons.c sdl_led.c

        badge_firmware/utils.c
        badge_firmware/assetList.c
        badge_firmware/badge_menu.c

        badge_firmware/badge_apps/badgelandia.c
        badge_firmware/badge_apps/badgey_bird.c
        badge_firmware/badge_apps/groundwar.c
        #badge_firmware/badge_apps/screensaver.c
        #badge_firmware/badge_apps/boot_splash.c
        )

add_definitions(-DSDL_BADGE)
find_package(SDL2 REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lm")

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS}
                      badge_firmware/include
                      badge_firmware/assets
                      badge_firmware/assets/lores
                    .)

add_executable(BasicSDLBadge ${SOURCE_FILES})

target_link_libraries(BasicSDLBadge PUBLIC SDL2::SDL2)
# If CMake chokes on the above 'PUBLIC SDL2::SDL2' or doesn't link
#target_link_libraries(BasicSDLBadge ${SDL2_LIBRARIES})

