cmake_minimum_required(VERSION 3.31)
project(Anime4K_GUI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)
set(ASSET_MANIFEST ${CMAKE_BINARY_DIR}/asset_manifest.txt)
set(PAK_FILE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets.pak)

file(GLOB_RECURSE ASSET_FILES "${ASSETS_DIR}/*")

add_custom_command(
        OUTPUT ${ASSET_MANIFEST}
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/generate_asset_manifest.py ${ASSETS_DIR} ${ASSET_MANIFEST}
        DEPENDS ${CMAKE_SOURCE_DIR}/generate_asset_manifest.py
        COMMENT "Generating asset manifest"
)

add_custom_command(
        OUTPUT ${PAK_FILE}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/pack_assets.py ${ASSETS_DIR} ${PAK_FILE}
        DEPENDS ${ASSET_MANIFEST} ${ASSET_FILES} ${CMAKE_SOURCE_DIR}/pack_assets.py
        COMMENT "Packing assets into custom .pak file"
)

add_custom_target(pack_assets ALL DEPENDS ${PAK_FILE})

add_subdirectory(lib/glad)
add_subdirectory(lib/glfw)
add_subdirectory(lib/imgui)
add_subdirectory(lib/freetype)
add_subdirectory(lib/spdlog)
add_subdirectory(lib/tiny-process-library)

include_directories(lib/glad/include)
include_directories(lib/glfw/include)
include_directories(lib/imgui)
include_directories(lib/freetype/include)
include_directories(lib/spdlog/include)
include_directories(lib/json)
include_directories(lib/tiny-process-library)
include_directories(src)

add_executable(Anime4K_GUI src/main.cpp
        src/pch.h
        src/Utilities/WindowUtilities.h
        src/Utilities/AssetLoader.h
        src/Utilities/Logger.h
        src/UI/Renderer.cpp
        src/UI/Renderer.h
        src/Data/Video.h
        src/Data/Encoder.h
        src/Data/Shader.h
        src/Data/Resolution.h
        src/Utilities/Utilities.h
        src/VideoLoader.cpp
        src/VideoLoader.h
        src/Data/Configuration.cpp
        src/Data/Configuration.h
)

add_dependencies(${PROJECT_NAME} pack_assets)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.h)
target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw imgui freetype spdlog tiny-process-library)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE dwmapi)
endif()