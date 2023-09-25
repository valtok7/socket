cmake_minimum_required(VERSION 3.0)
project(SocketServer)

set(CMAKE_CXX_STANDARD 20)

# cppファイルをビルド対象に加える
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 実行ファイル作る
add_executable(SocketServer ${SOURCES})


if (WIN32)
    # Windows用のライブラリリンク
    target_link_libraries(SocketServer wsock32 ws2_32)

    # Windows用のコンパイルオプション
    add_compile_options(-D _NOSOCKLEN)
else()
    # Linux用のライブラリリンク
    target_link_libraries(SocketServer pthread)
endif()
