//
// Created by lenovo on 7/31/2025.
//
#include "bitboard.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "constants.h"
#include "utils.h"

void main() {
    const char* FEN_start = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Bitboard board = init_Bitboard(FEN_start);
    print_board(board);
}