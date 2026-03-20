cmake_minimum_required(VERSION 3.5)
project(8051Tutorial)

set(CMAKE_CXX_STANDARD 20)

set(SRC_FILES
        src/main.cpp
        src/Game.cpp
        src/TextureManager.cpp
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
)

# ALWAYS create the executable first
add_executable(8051Tutorial ${SRC_FILES})

# =========================
# Windows Setup
# =========================
if (WIN32)

    set(SDL3_SDK_ROOT "${CMAKE_SOURCE_DIR}/SDL3_SDK_WIN32")
    set(SDL3_IMAGE_PATH "${CMAKE_SOURCE_DIR}/SDL3_Image")

    target_include_directories(8051Tutorial PRIVATE
            ${SDL3_SDK_ROOT}/include
            ${SDL3_IMAGE_PATH}/include
    )

    target_link_directories(8051Tutorial PRIVATE
            ${SDL3_SDK_ROOT}/lib
            ${SDL3_IMAGE_PATH}/lib
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
if (APPLE)

    # Allow CMake to find all Homebrew packages
    set(CMAKE_PREFIX_PATH "/opt/homebrew")

    find_package(SDL3 REQUIRED)
    find_package(SDL3_image REQUIRED)
    find_package(SDL3_ttf REQUIRED)

    target_link_libraries(8051Tutorial
            PRIVATE
            SDL3::SDL3
            SDL3_image::SDL3_image
            SDL3_ttf::SDL3_ttf
    )

    target_compile_options(8051Tutorial PRIVATE -Wno-narrowing)
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