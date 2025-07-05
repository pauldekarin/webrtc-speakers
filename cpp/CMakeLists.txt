# cmake_minimum_required(VERSION 3.10)

# project(webrtc-speakers LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(SOURCE_DIR "${CMAKE_SOURCE_DIR}")
# set(PORTAUDIO_DIR "${CMAKE_SOURCE_DIR}/portaudio")
# set(PULSEAUDIO_DIR "${CMAKE_SOURCE_DIR}/pulseaudio")
# set(PULSEAUDIO_BUILD_DIR "${PULSEAUDIO_DIR}/build")
# set(WEBRTC_DIR "${CMAKE_SOURCE_DIR}/webrtc/src")

# set(SOURCES 
#     ${SOURCE_DIR}/main.cpp
# )

# if(NOT EXISTS ${PULSEAUDIO_BUILD_DIR})
#     message(FATAL_ERROR "Build pulseaudio first!")
# endif()
#
# # ==== WEBRTC LIBRARIES =====
# find_library(WEBRTC_LIB )
# # ==== WEBRTC LIBRARIES =====
#
# # <                             >
#
# # ==== PULSE AUDIO LIBRARIES =====
# find_library(PULSEAUDIO_LIB NAMES pulse libpulse PATHS "${PULSEAUDIO_BUILD_DIR}/src/pulse")
# if(NOT PULSEAUDIO_LIB)
#     message(FATAL_ERROR "Cannot resolve PulseAudio shared libraries!")
# endif()

# find_library(PULSEAUDIO_SIMPLE_LIB NAMES pulse-simple libpulse-simple PATHS "${PULSEAUDIO_BUILD_DIR}/src/pulse")
# if(NOT PULSEAUDIO_SIMPLE_LIB)
#     message(FATAL_ERROR "Cannot resolve PulseAudio Simple shared libraries!")
# endif()
# # ==== PULSE AUDIO LIBRARIES =====

# # <                             >

# # ==== PULSE AUDIO HEADERS =======
# find_path(PULSEAUDIO_BUILD_INCLUDE_DIR version.h ${PULSEAUDIO_BUILD_DIR}/src/pulse)
# if(PULSEAUDIO_BUILD_INCLUDE_DIR STREQUAL "PULSEAUDIO_BUILD_INCLUDE_DIR-NOTFOUND")
#     message(FATAL_ERROR " pulseaudio build ${PULSEAUDIO_BUILD_DIR} does not contain required header files!")
# else()
#     set(PULSEAUDIO_BUILD_INCLUDE_DIR ${PULSEAUDIO_BUILD_DIR}/src)
# endif()

# find_path(PULSEAUDIO_INCLUDE_DIR simple.h PATHS ${PULSEAUDIO_DIR}/src/pulse)
# if(PULSEAUDIO_INCLUDE_DIR STREQUAL "PULSEAUDIO_INCLUDE_DIR-NOTFOUND")
#     message(FATAL_ERROR "pulse audio include does not found!")
# else()
#     set(PULSEAUDIO_INCLUDE_DIR ${PULSEAUDIO_DIR}/src)
# endif()
# # ==== PULSE AUDIO HEADERS =======
#
# # <                             >
#
#
# find_library(PORTAUDIO_LIB NAMES portaudio libportaudio PATHS ${PORTAUDIO_LIB}/lib)
# find_path(PORTAUDIO_INCLUDE_DIR portaudio.h PATHS ${PORTAUDIO_DIR}/include)


# # message(FATAL_ERROR ${PULSEAUDIO_LIBRARIES})

# add_compile_options(-lpulse)

# add_executable(${PROJECT_NAME} ${SOURCES})

# target_include_directories(${PROJECT_NAME} PRIVATE 
#     ${PULSEAUDIO_BUILD_INCLUDE_DIR}
#     ${PULSEAUDIO_INCLUDE_DIR}
#     ${PORTAUDIO_INCLUDE_DIR})

# target_link_libraries(${PROJECT_NAME} PRIVATE 
#     ${PULSEAUDIO_LIB}
#     ${PULSEAUDIO_SIMPLE_LIB}
#     ${PORTAUDIO_LIB})

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

cmake_minimum_required(VERSION 3.10)
project(webrtc-speakers CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")

# Явное указание пути к OpenSSL
set(OPENSSL_ROOT_DIR /usr/local/ssl)
set(OPENSSL_INCLUDE_DIR /usr/local/ssl/include)
set(OPENSSL_LIBRARIES /usr/local/ssl/lib)
find_package(OpenSSL REQUIRED)

set(WEBRTC_OUT_DIR "${CMAKE_SOURCE_DIR}/webrtc/src/out/Default")
set(WEBRTC_DIR "${CMAKE_SOURCE_DIR}/webrtc/src")
set(JSON_DIR "${CMAKE_SOURCE_DIR}/nlohmann_json")

add_subdirectory(${JSON_DIR})

add_compile_options(
        -DWEBRTC_POSIX=1
        -DWEBRTC_LINUX=1
        -fpermissive
)

# Libwebsockets configuration
# Disable BoringSSL and use OpenSSL instead, as BoringSSL is not configured
set(LWS_WITH_BORINGSSL OFF CACHE BOOL "Disable BoringSSL, use OpenSSL" FORCE)
set(LWS_WITH_SSL ON CACHE BOOL "Enable SSL support" FORCE)
set(LWS_WITH_SHARED ON CACHE BOOL "Build shared library" FORCE)
# Disable Werror for shorten-64-to-32 to avoid treating this warning as an error
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=shorten-64-to-32" CACHE STRING "C compiler flags" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/libwebsockets)

# WebRTC
find_library(WEBRTC_LIB libwebrtc.a PATHS ${WEBRTC_OUT_DIR}/obj)
if(NOT WEBRTC_LIB)
    message(FATAL_ERROR "Cannot find libwebrtc.a at ${WEBRTC_OUT_DIR}/obj")
endif()

# Compile definitions
add_compile_options(
        -DWEBRTC_POSIX=1
        -DWEBRTC_LINUX=1
        -fno-rtti
        -g -O0
        -fpermissive
)

add_executable(${PROJECT_NAME} app.cpp signaling_client.cpp peer_connection.cpp audio_source.cpp video_track_source.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${WEBRTC_DIR}
        ${WEBRTC_DIR}/third_party/abseil-cpp/
        ${nlohmann_json_INCLUDE_BUILD_DIR}
        ${CMAKE_SOURCE_DIR}/libwebsockets/include
        ${CMAKE_SOURCE_DIR}/signaling_client.hpp
        ${CMAKE_SOURCE_DIR}/audio_source.hpp
        ${CMAKE_SOURCE_DIR}/video_track_source.hpp
        ${CMAKE_SOURCE_DIR}/peer_connection.hpp
        ${CMAKE_SOURCE_DIR}/audio_capture_module.hpp
        ${CMAKE_SOURCE_DIR}/webrtc_includes.h
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        ${WEBRTC_LIB}
        websockets_shared
        OpenSSL::SSL
        OpenSSL::Crypto
)