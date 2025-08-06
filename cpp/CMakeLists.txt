cmake_minimum_required(VERSION 3.16)
project(webrtc-speakers CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")

find_package(libwebsockets REQUIRED)


add_subdirectory(${CMAKE_SOURCE_DIR}/spdlog)
add_subdirectory(${CMAKE_SOURCE_DIR}/portaudio)
add_subdirectory(${CMAKE_SOURCE_DIR}/nlohmann_json)

set(LWS_WITH_SHARED ON CACHE BOOL "Build shared library" FORCE)
set(OPENSSL_INCLUDE_DIRS "/usr/include/openssl")
set(OPENSSL_LIBRARIES "/usr/lib/x86_64-linux-gnu/libcrypto.so:/usr/lib/x86_64-linux-gnu/libssl.so")

set(WEBRTC_DIR "${CMAKE_SOURCE_DIR}/webrtc-native-build/dist/Release/webrtc-native-build-7204.e4445e.163/")

find_library(WEBRTC_LIB libwebrtc.a PATHS ${CMAKE_SOURCE_DIR}/webrtc-native/src/out/Default/obj)
if (NOT WEBRTC_LIB)
    message(FATAL_ERROR "Not found WebRTC Libraries!")
endif ()

add_compile_options(
        -DWEBRTC_POSIX=1
)

add_executable(${PROJECT_NAME}
        app.cpp
        rtc_client.cpp
        video_track_source.cpp
        audio_source.cpp
        signaling_client.cpp
        conductor.cpp
        audio_capture_module.cpp
        audio_feeder.cpp
        audio_feeder.hpp
        audio_handler_interface.hpp
        audio_device_info.cpp
        audio_device_info.hpp
        audio_config.cpp
        audio_config.hpp
        settings.cpp
        settings.hpp
        virtual_audio_output.cpp
        virtual_audio_output.hpp
        dummy_rtc_client.cpp
        dummy_rtc_client.hpp
)

file(GLOB M1 "${CMAKE_SOURCE_DIR}/webrtc-native/src/out/Default/obj/buildtools/third_party/libc++/libc++/*.o")
file(GLOB M2 "${CMAKE_SOURCE_DIR}/webrtc-native/src/out/Default/obj/buildtools/third_party/libc++abi/libc++abi/*.o")

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${X11_LIBRARIES}
        ${WEBRTC_LIB}
        websockets_shared
        nlohmann_json
        portaudio
        spdlog
        ${M1}
        ${M2}
        pulse
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
        #        ${CMAKE_SOURCE_DIR}/
        ${CMAKE_SOURCE_DIR}/nlohmann_json/include
        ${CMAKE_SOURCE_DIR}/libwebsockets/include
        ${X11_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/webrtc-native/src/
        ${CMAKE_SOURCE_DIR}/webrtc-native/src/third_party/abseil-cpp
        ${CMAKE_SOURCE_DIR}/portaudio/include
        ${CMAKE_SOURCE_DIR}/spdlog/include
)


