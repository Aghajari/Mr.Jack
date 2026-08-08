cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME Mr.Jack)

project(${PROJECT_NAME} C)

# --------------------------------------------------
# Source files
# --------------------------------------------------

add_executable(${PROJECT_NAME}
        main.c

        linked_list/linked_list.c
        linked_list/linked_list.h

        graphic/GUI_init.c
        graphic/GUI_init.h
        graphic/GUI_gravity.h
        graphic/MrJack_Map.c
        graphic/MrJack_Map.h

        pages/starter.c
        pages/starter.h
        pages/start_game.c
        pages/start_jack.c
        pages/start_map.c

        audio/audio.c
        audio/audio.h

        characters/characters.h
        characters/characters.c
        characters/MissStealthy.c
        characters/InspecteurLestrade.c
        characters/inner_characters.h
        characters/JermyBert.c
        characters/JohnWatson.c
        characters/SherlockHolmes.c
        characters/JohnSmith.c
        characters/SergentGoodley.c
        characters/SirWilliamGull.c

        gameplay/MrJack_Gameplay.h
        gameplay/MrJack_Gameplay.c
)

# --------------------------------------------------
# Find Homebrew
# --------------------------------------------------

execute_process(
        COMMAND brew --prefix sdl2
        OUTPUT_VARIABLE SDL2_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND brew --prefix sdl2_ttf
        OUTPUT_VARIABLE SDL2_TTF_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND brew --prefix sdl2_image
        OUTPUT_VARIABLE SDL2_IMAGE_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "SDL2       = ${SDL2_PREFIX}")
message(STATUS "SDL2_ttf   = ${SDL2_TTF_PREFIX}")
message(STATUS "SDL2_image = ${SDL2_IMAGE_PREFIX}")

# --------------------------------------------------
# Include directories
# --------------------------------------------------

target_include_directories(${PROJECT_NAME} PRIVATE
        "${SDL2_PREFIX}/include"
        "${SDL2_PREFIX}/include/SDL2"

        "${SDL2_TTF_PREFIX}/include"
        "${SDL2_IMAGE_PREFIX}/include"
)

# --------------------------------------------------
# Libraries
# --------------------------------------------------

target_link_libraries(${PROJECT_NAME} PRIVATE
        "${SDL2_PREFIX}/lib/libSDL2.dylib"
        "${SDL2_TTF_PREFIX}/lib/libSDL2_ttf.dylib"
        "${SDL2_IMAGE_PREFIX}/lib/libSDL2_image.dylib"
)

# --------------------------------------------------
# Assets
# --------------------------------------------------

file(MAKE_DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/assets"
)

file(GLOB ASSETS
        "assets/*.*"
        "assets_rain/*.*"
        "assets_cards/*.*"
        "assets_round/*.*"
        "assets_items/*.*"
)

foreach(NAME ${ASSETS})
    configure_file(
            "${NAME}"
            "${CMAKE_CURRENT_BINARY_DIR}/assets"
            COPYONLY
    )
endforeach()