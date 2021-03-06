cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME Mr.Jack)
project(${PROJECT_NAME} C)

set(SDL2_DIR ${CMAKE_C_IMPLICIT_LINK_FRAMEWORK_DIRECTORIES})

add_executable(${PROJECT_NAME} main.c
        linked_list/linked_list.c
        linked_list/linked_list.h
        graphic/GUI_init.c
        graphic/GUI_init.h
        graphic/GUI_gravity.h
        pages/starter.c
        pages/starter.h
        pages/start_game.c
        pages/start_jack.c
        audio/audio.c
        audio/audio.h
        characters/characters.h
        characters/characters.c
        pages/start_map.c
        graphic/MrJack_Map.h
        graphic/MrJack_Map.c
        gameplay/MrJack_Gameplay.h
        gameplay/MrJack_Gameplay.c
        characters/MissStealthy.c
        characters/InspecteurLestrade.c
        characters/inner_characters.h
        characters/JermyBert.c
        characters/JohnWatson.c
        characters/SherlockHolmes.c
        characters/JohnSmith.c
        characters/SergentGoodley.c
        characters/SirWilliamGull.c)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assets)
file(GLOB ASSETS "assets/*.*" "assets_rain/*.*" "assets_cards/*.*" "assets_round/*.*" "assets_items/*.*")
foreach( name ${ASSETS} )
    configure_file(${name} ${CMAKE_CURRENT_BINARY_DIR}/assets COPYONLY)
endforeach( name ${ASSETS} )

target_link_libraries(${PROJECT_NAME} ${SDL2_DIR}/SDL2.framework/SDL2)
target_link_libraries(${PROJECT_NAME} ${SDL2_DIR}/SDL2_ttf.framework/SDL2_ttf)
target_link_libraries(${PROJECT_NAME} ${SDL2_DIR}/SDL2_image.framework/SDL2_image)
#target_link_libraries(${PROJECT_NAME} ${SDL2_DIR}/SDL2_mixer.framework/SDL2_mixer)
