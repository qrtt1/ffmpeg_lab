cmake_minimum_required(VERSION 2.8)

project(fflab)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)

find_package(FFMPEG)
find_package(ZLIB)
find_package(BZIP2)
find_package(X264)

include_directories(fflab ${FFMPEG_INCLUDE_DIR})

add_executable(fflab ${PROJECT_SOURCE_DIR}/src/main.c)

target_link_libraries(fflab ${FFMPEG_LIBRARIES} ${ZLIB_LIBRARIES})
target_link_libraries(fflab ${X264_LIBRARIES})
target_link_libraries(fflab ${BZIP2_LIBRARIES})
target_link_libraries(fflab "/opt/muzee/lib/libmp3lame.dylib")
target_link_libraries(fflab "-framework CoreFoundation -framework VideoDecodeAcceleration -framework QuartzCore")
