cmake_minimum_required(VERSION 4.1)
project(8051Tutorial)

set(CMAKE_CXX_STANDARD 20)

#Set SDL3 Include and lib paths
set(SRC_FILES
        src/main.cpp
        #add all new source files here
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
        src/manager/Scene.cpp
)

if (WIN32)
    set(SDL3_SDK_ROOT "${CMAKE_SOURCE_DIR}/SDL3_SDK_WIN32")
    set(SDL3_IMAGE_PATH "${CMAKE_SOURCE_DIR}/SDL3_Image")
    set(SDL3_INCLUDE_DIR "${SDL3_SDK_ROOT}/include")
    set(SDL3_LIB_DIR "${SDL3_SDK_ROOT}/lib")

    file(GLOB SDL3_DLLS "${SDL3_SDK_ROOT}/bin/*.dll")
    file(COPY ${SDL3_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

endif ()

include_directories(
        ${SDL3_INCLUDE_DIR}
        ${SDL3_IMAGE_PATH}/include
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/utils
        ${PROJECT_SOURCE_DIR}/src/ecs
        ${PROJECT_SOURCE_DIR}/src/ecs/system
        ${PROJECT_SOURCE_DIR}/src/vendor
        #add all new directories here
)
link_directories(
        ${SDL3_LIB_DIR}
        ${SDL3_IMAGE_PATH}/lib
)

add_executable(8051Tutorial ${SRC_FILES})

target_link_libraries(8051Tutorial
        PRIVATE
        SDL3
        SDL3_image
        SDL3_ttf
        SDL3_mixer
)

if (APPLE)
    set_target_properties(8051Tutorial PROPERTIES
            BUILD_RPATH "${SDL3_LIB_DIR}"
    )

    target_compile_options(8051Tutorial PRIVATE -Wno-narrowing)
endif ()