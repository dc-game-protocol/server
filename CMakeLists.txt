cmake_minimum_required(VERSION 3.16.3)
project("Tic Tac Toe")

set(CMAKE_C_STANDARD 11)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_library(tictactoe tictactoe_trans.c tictactoe_trans.h tictactoe_env.h tictactoe_env.c)
target_compile_options(tictactoe PRIVATE -Wpedantic -Wall -Wextra)

add_executable(server server.c helper.h helper.c)

target_link_libraries(server dc dc_fsm tictactoe)
target_compile_options(server PRIVATE -g -Wpedantic -Wall -Wextra)
