cmake_minimum_required(VERSION 3.16.3)
project("Tic Tac Toe")

set(CMAKE_C_STANDARD 11)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_library(ttt games/tictactoe/tictactoe_trans.c games/tictactoe/tictactoe_trans.h games/tictactoe/tictactoe_env.h games/tictactoe/tictactoe_env.c)
target_compile_options(ttt PRIVATE -Wpedantic -Wall -Wextra)

add_library(rps games/rockpaperscissors/rockpaperscissors_env.h games/rockpaperscissors/rockpaperscissors_env.c games/rockpaperscissors/rockpaperscissors_trans.h games/rockpaperscissors/rockpaperscissors_trans.c)
target_compile_options(rps PRIVATE -Wpedantic -Wall -Wextra)

add_library(tcp TCP/request.c TCP/request.h TCP/response_status.h TCP/response.c TCP/response.h)
target_compile_options(tcp PRIVATE -Wpedantic -Wall -Wextra)

add_executable(server server.c server/helper.h server/helper.c server/server_env.c server/server_env.h server/client.c server/client.h server/game_env.c server/game_env.h fsm_local/fsm.h fsm_local/fsm.c)

target_link_libraries(server dc dc_fsm  tcp rps ttt)
target_compile_options(server PRIVATE -g -Wpedantic -Wall -Wextra)
