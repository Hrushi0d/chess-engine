cmake_minimum_required(VERSION 3.10)
project(ChessEngine C)

set(CMAKE_C_STANDARD 99)

add_executable(chess
        main.c
        bitboard.c
        utils.c
)