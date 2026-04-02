cmake_minimum_required(VERSION 3.5)
project(8051Tutorial)

set(CMAKE_CXX_STANDARD 20)

set(SRC_FILES
        src/main.cpp
        src/Game.cpp
        src/manager/TextureManager.cpp
        src/Map.cpp
        src/GameObject.cpp
        src/utils/Vector2D.cpp
        src/utils/Collision.cpp
        src/ecs/system/CollisionSystem.cpp
        src/ecs/World.cpp
        src/vendor/tinyxml2.cpp
        src/manager/AssetManager.cpp
        src/scene/Scene.cpp
        src/ecs/system/EventResponseSystem.cpp
        src/ecs/system/MainMenuSystem.cpp
        src/ecs/system/KeyboardInputSystem.cpp
        src/ecs/system/RenderSystem.cpp
        src/ecs/system/SpawnerSystem.cpp
        src/ecs/system/LevelUpSystem.hpp
        src/ecs/system/GridSystem.cpp
        src/ecs/system/MovementSystem.cpp
        src/manager/WeaponManager.cpp
        src/behaviours/WeaponBehaviours.cpp
        src/utils/Timer.cpp
        src/manager/ItemManager.hpp
        src/manager/ItemManager.cpp
        src/ecs/system/LevelUpSystem.cpp
        src/ecs/system/WeaponFireSystem.cpp
        src/utils/data/Stat.h
        src/ecs/system/LevelUpHandler.h
        src/ecs/system/LevelUpHandler.cpp
        src/ecs/system/MouseInputSystem.cpp
        src/ecs/system/PlayerStatListener.h
        src/ecs/system/PlayerStatListener.cpp
        src/ecs/system/PickUpSystem.cpp
        src/ecs/system/DebugRenderSystem.cpp
        src/manager/AudioManager.cpp
        src/ecs/system/HealthSystem.cpp
        src/ecs/system/OverlayRenderSystem.cpp
        src/ecs/system/GameStateSystem.cpp
        src/ecs/system/HUDSystem.cpp
)

# ALWAYS create the executable first
add_executable(8051Tutorial ${SRC_FILES})

# =========================
# Windows Setup
# =========================
if (WIN32)

    set(SDL3_SDK_ROOT "${CMAKE_SOURCE_DIR}/SDL3_SDK_WIN32")
    set(SDL3_IMAGE_PATH "${CMAKE_SOURCE_DIR}/SDL3_Image")
    set(SDL3_TTF_PATH "${CMAKE_SOURCE_DIR}/SDL3_TTF")
    set(SDL3_MIXER_PATH "${CMAKE_SOURCE_DIR}/SDL3_Mixer")

    target_include_directories(8051Tutorial PRIVATE
            ${SDL3_SDK_ROOT}/include
            ${SDL3_IMAGE_PATH}/include
            ${SDL3_TTF_PATH}/include
            ${SDL3_MIXER_PATH}/include
    )

    target_link_directories(8051Tutorial PRIVATE
            ${SDL3_SDK_ROOT}/lib
            ${SDL3_IMAGE_PATH}/lib
            ${SDL3_TTF_PATH}/lib
            ${SDL3_MIXER_PATH}/lib
    )

    file(GLOB SDL3_DLLS "${SDL3_SDK_ROOT}/bin/.dll")
    file(GLOB SDL3_IMAGE_DLLS "${SDL3_IMAGE_PATH}/bin/.dll")

    file(COPY ${SDL3_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${SDL3_IMAGE_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

    target_link_libraries(8051Tutorial
            PRIVATE
            SDL3
            SDL3_image
            SDL3_ttf
            SDL3_mixer
    )
endif()

# =========================
# macOS (Homebrew) Setup
# =========================
# =========================
# macOS (Homebrew + custom SDL3_mixer) Setup
# =========================
if(APPLE)

    # Homebrew SDL3 packages for core, image, ttf
    find_package(SDL3 REQUIRED)
    find_package(SDL3_image REQUIRED)
    find_package(SDL3_ttf REQUIRED)

    # Include headers for Homebrew SDL3
    target_include_directories(8051Tutorial PRIVATE
            ${SDL3_INCLUDE_DIRS}
            ${SDL3_IMAGE_INCLUDE_DIRS}
            ${SDL3_TTF_INCLUDE_DIRS}
    )

    # Include headers for your custom SDL3_mixer
    set(SDL3_MIXER_ROOT "${CMAKE_SOURCE_DIR}/SDL3_SDK_APPLE")
    target_include_directories(8051Tutorial PRIVATE
            ${SDL3_MIXER_ROOT}/include
    )

    # Link Homebrew libraries
    target_link_libraries(8051Tutorial PRIVATE
            SDL3::SDL3
            SDL3_image::SDL3_image
            SDL3_ttf::SDL3_ttf
    )

    # Link custom SDL3_mixer
    target_link_libraries(8051Tutorial PRIVATE
            "${SDL3_MIXER_ROOT}/lib/libSDL3_mixer.dylib"
    )

    # Embed rpath so executable can find custom mixer at runtime
    set_target_properties(8051Tutorial PROPERTIES
            BUILD_RPATH "${SDL3_MIXER_ROOT}/lib"
            INSTALL_RPATH "${SDL3_MIXER_ROOT}/lib"
            MACOSX_RPATH TRUE
    )
endif()

# =========================
# Project Includes
# =========================
target_include_directories(8051Tutorial PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/utils
        ${PROJECT_SOURCE_DIR}/src/ecs
        ${PROJECT_SOURCE_DIR}/src/ecs/system
        ${PROJECT_SOURCE_DIR}/src/vendor
        ${PROJECT_SOURCE_DIR}/src/behaviours
)