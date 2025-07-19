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

#cmake_minimum_required(VERSION 3.10)
#project(webrtc-speakers CXX)
#
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
## Явное указание пути к OpenSSL
##set(OPENSSL_ROOT_DIR /usr/local/ssl)
##set(OPENSSL_INCLUDE_DIR /usr/local/ssl/include)
##set(OPENSSL_LIBRARIES /usr/local/ssl/lib)
#
#add_subdirectory(${CMAKE_SOURCE_DIR}/boringssl)
#
#set(OPENSSL_LIBRARIES "/usr/boringssl/lib/libcrypto.so:/usr/boringssl/lib/libssl.so")
#set(OPENSSL_INCLUDE_DIRS "/usr/boringssl/include")
#set(WEBRTC_OUT_DIR "${CMAKE_SOURCE_DIR}/webrtc-native/src/out/Default")
#set(WEBRTC_DIR "${CMAKE_SOURCE_DIR}/webrtc-native/src")
#set(JSON_DIR "${CMAKE_SOURCE_DIR}/nlohmann_json")
#
#add_subdirectory(${JSON_DIR})
#
#add_compile_options(
#        -DWEBRTC_POSIX=1
#        -DWEBRTC_LINUX=1
#        -fpermissive
#)
#
#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang++)
##add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=shorten-64-to-32")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fuse-ld=lld -m64")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
#
## Libwebsockets configuration
#
## Disable BoringSSL and use OpenSSL instead, as BoringSSL is not configured
#
#set(CMAKE_CXX_COMPILER /usr/bin/clang++)
## Disable Werror for shorten-64-to-32 to avoid treating this warning as an error
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=shorten-64-to-32" CACHE STRING "C compiler flags" FORCE)
#
##add_subdirectory(${CMAKE_SOURCE_DIR}/libwebsockets)
##find_package(Libwebsockets REQUIRED)
#
## Libwebsockets
#set(LWS_WITH_BORINGSSL ON CACHE BOOL "Use BoringSSL" FORCE)
#set(LWS_WITH_SSL ON CACHE BOOL "Enable SSL support" FORCE)
#set(LWS_WITH_SHARED ON CACHE BOOL "Build shared library" FORCE)
#add_subdirectory(${CMAKE_SOURCE_DIR}/libwebsockets)
#add_compile_options(-Wno-sign-conversion)
#set_target_properties(websockets websockets_shared PROPERTIES
#        COMPILE_FLAGS "-Wno-error -Wno-sign-conversion -Wno-sign-compare -Wno-shorten-64-to-32"
#)
## WebRTC
#find_library(WEBRTC_LIB libwebrtc.a PATHS ${WEBRTC_OUT_DIR}/obj)
#if(NOT WEBRTC_LIB)
#    message(FATAL_ERROR "Cannot find libwebrtc.a at ${WEBRTC_OUT_DIR}/obj")
#endif()
#
## Compile definitions
#add_compile_options(
#        -DWEBRTC_POSIX=1
#        -DWEBRTC_LINUX=1
#        -fno-rtti
#        -g -O0
#        -fpermissive
#)
#
#add_executable(${PROJECT_NAME} app.cpp audio_source.cpp signaling_client.cpp video_track_source.cpp peer_connection.cpp)
#
#target_include_directories(${PROJECT_NAME} PRIVATE
#        ${WEBRTC_DIR}
#        ${WEBRTC_DIR}/third_party/abseil-cpp/
#        ${nlohmann_json_INCLUDE_BUILD_DIR}
#        ${CMAKE_SOURCE_DIR}/libwebsockets/include
#        ${CMAKE_SOURCE_DIR}/signaling_client.hpp
#        ${CMAKE_SOURCE_DIR}/audio_source.hpp
#        ${CMAKE_SOURCE_DIR}/video_track_source.hpp
#        ${CMAKE_SOURCE_DIR}/peer_connection.hpp
#        ${CMAKE_SOURCE_DIR}/audio_capture_module.hpp
#        ${CMAKE_SOURCE_DIR}/webrtc_includes.h
#)
#
#target_link_libraries(${PROJECT_NAME} PUBLIC
#        ${WEBRTC_LIB}
#        websockets
#        websockets_shared
#)
#
#cmake_minimum_required(VERSION 3.16)
#project(webrtc-speakers CXX)
#
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_C_COMPILER /usr/bin/clang)
#set(CMAKE_CXX_COMPILER /usr/bin/clang++)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
#
#find_package(X11 REQUIRED)
#find_package(libwebsockets REQUIRED)
#
#add_subdirectory(${CMAKE_SOURCE_DIR}/nlohmann_json)
#
#set(LWS_WITH_SHARED ON CACHE BOOL "Build shared library" FORCE)
#set(OPENSSL_INCLUDE_DIRS "/usr/include/openssl")
#set(OPENSSL_LIBRARIES "/usr/lib/x86_64-linux-gnu/libcrypto.so:/usr/lib/x86_64-linux-gnu/libssl.so")
#
#set(WEBRTC_DIR "${CMAKE_SOURCE_DIR}/webrtc-native-build/dist/Release/webrtc-native-build-7204.e4445e.163/")
#
#find_library(WEBRTC_LIB libwebrtc.a PATHS ${CMAKE_SOURCE_DIR}/webrtc-native-build/3rdParty/webrtc_native/webrtc/src/out/Linux-x64/obj)
#if(NOT WEBRTC_LIB)
#    message(FATAL_ERROR "Not found WebRTC Libraries!")
#endif ()
#
#add_compile_options(
#        -DWEBRTC_POSIX=1
#)
#
#add_executable(${PROJECT_NAME}
#        app.cpp
#        peer_connection.cpp
#        video_track_source.cpp
#        audio_source.cpp
#        signaling_client.cpp
#        conductor.cpp
#        rtcp.cpp
#)
#
#target_link_libraries(${PROJECT_NAME} PRIVATE
#        ${X11_LIBRARIES}
#        ${WEBRTC_LIB}
#        websockets_shared
#        nlohmann_json
#)
#
#target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
#        ${CMAKE_SOURCE_DIR}/nlohmann_json/include
#        ${CMAKE_SOURCE_DIR}/libwebsockets/include
#        ${X11_INCLUDE_DIR}
#        ${CMAKE_SOURCE_DIR}/webrtc-native-build/3rdParty/webrtc_native/webrtc/src
#        ${CMAKE_SOURCE_DIR}/webrtc-native-build/3rdParty/webrtc_native/webrtc/src/third_party/abseil-cpp
#)












cmake_minimum_required(VERSION 3.16)
project(webrtc-speakers CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")

add_subdirectory(${CMAKE_SOURCE_DIR}/nlohmann_json)
find_package(X11 REQUIRED)


find_library(WEBRTC_LIB libwebrtc.a PATHS ${CMAKE_SOURCE_DIR}/webrtc-native-build/3rdParty/webrtc_native/webrtc/src/out/Linux-x64/obj)
if(NOT WEBRTC_LIB)
    message(FATAL_ERROR "Not found WebRTC Libraries!")
endif ()

add_compile_options(
        -DWEBRTC_POSIX=1
)

add_executable(${PROJECT_NAME}
        peer_connection.cpp
        video_track_source.cpp
        audio_source.cpp
        conductor.cpp
        rtcp.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${WEBRTC_LIB}
        ${X11_LIBRARIES}
        nlohmann_json
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
        ${CMAKE_SOURCE_DIR}/nlohmann_json/include
        ${X11_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/webrtc-native-build/3rdParty/webrtc_native/webrtc/src
        ${CMAKE_SOURCE_DIR}/webrtc-native-build/3rdParty/webrtc_native/webrtc/src/third_party/abseil-cpp
)