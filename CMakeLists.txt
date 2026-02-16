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
        COMMAND python3 ${CMAKE_SOURCE_DIR}/generate_asset_manifest.py ${ASSETS_DIR} ${ASSET_MANIFEST}
        DEPENDS ${CMAKE_SOURCE_DIR}/generate_asset_manifest.py
        COMMENT "Generating asset manifest"
)

add_custom_command(
        OUTPUT ${PAK_FILE}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND python3 ${CMAKE_SOURCE_DIR}/pack_assets.py ${ASSETS_DIR} ${PAK_FILE}
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
        src/Utilities/WindowsUtilities.h
        src/Utilities/AssetLoader.h
        src/Utilities/Logger.h
        src/UI/Renderer.cpp
        src/UI/Renderer.h
        src/Data/Video.h
        src/Data/Encoder.h
        src/Data/Shader.h
        src/Data/Resolution.h
        src/Utilities/Utilities.h
        src/Video/VideoLoader.cpp
        src/Video/VideoLoader.h
        src/Data/Data.cpp
        src/Data/Data.h
        src/App.cpp
        src/App.h
        src/Utilities/AssetLoader.cpp
        src/Hardware/GpuDetector.h
        src/Hardware/GpuDetector.cpp
        src/UI/RendererUtilities.h
        src/Video/VideoProcessor.cpp
        src/Video/VideoProcessor.h
        src/Data/UpscalingStatus.h
        src/Data/Configuration.h
        src/Data/Configuration.cpp
)

add_dependencies(${PROJECT_NAME} pack_assets)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.h)
target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw imgui freetype spdlog tiny-process-library)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")

    set(ASSETS_FILE ${CMAKE_BINARY_DIR}/bin/assets.pak)
    set(ICNS_FILE resources/icon.icns)

    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE MACOSX_BUNDLE_ICON_FILE "icon.icns")

    set_source_files_properties(${ASSETS_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_source_files_properties(${ICNS_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    target_sources(${PROJECT_NAME} PRIVATE ${ASSETS_FILE})
    target_sources(${PROJECT_NAME} PRIVATE ${ICNS_FILE})

    add_custom_command(TARGET Anime4K_GUI POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/ffmpeg/macos"
            "$<TARGET_BUNDLE_CONTENT_DIR:Anime4K_GUI>/Resources"
            COMMENT "Copy FFMPEG to .app/Resources directory"
    )

    add_custom_command(TARGET Anime4K_GUI POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/shaders"
            "$<TARGET_BUNDLE_CONTENT_DIR:Anime4K_GUI>/Resources/shaders"
            COMMENT "Copy shaders to Resources directory"
    )

    add_custom_command(TARGET Anime4K_GUI POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove
            "${CMAKE_BINARY_DIR}/bin/assets.pak"
            COMMENT "Copy shaders to Resources directory"
    )

endif()

if(WIN32)
    target_link_options(Anime4K_GUI PRIVATE
            -static
            -static-libgcc
            -static-libstdc++
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE dwmapi setupapi)
    target_sources(Anime4K_GUI PRIVATE resources/appicon.rc)

    # Show console in Release mode
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set_target_properties(Anime4K_GUI PROPERTIES
                WIN32_EXECUTABLE TRUE
        )
    endif ()

    set(FFMPEG_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ffmpeg)
    add_custom_command(
            TARGET Anime4K_GUI
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${FFMPEG_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/ffmpeg/windows/ffmpeg.exe ${FFMPEG_DIRECTORY}/ffmpeg.exe
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/ffmpeg/windows/ffprobe.exe ${FFMPEG_DIRECTORY}/ffprobe.exe
            COMMENT "Copy ffmpeg binaries to output directory"
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/shaders"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders"
            COMMENT "Copy shaders to output directory"
    )
endif()