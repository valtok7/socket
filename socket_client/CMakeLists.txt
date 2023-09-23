cmake_minimum_required(VERSION 3.0)
project(SocketClient)

set(CMAKE_CXX_STANDARD 20)

# cppファイルをビルド対象に加える
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 実行ファイル作る
add_executable(SocketClient ${SOURCES})


if (WIN32)
    # Windows用のライブラリリンク
    target_link_libraries(SocketClient wsock32 ws2_32)
else()
    # Linux用のライブラリリンク
    target_link_libraries(SocketClient pthread)
endif()
