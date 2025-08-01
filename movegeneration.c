//
// Created by lenovo on 7/31/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "constants.h"

typedef struct {
    Move moves[MAX_MOVES];
    size_t count;
} MoveList;

// Placeholders for move generation
MoveList generate_white_pawn_moves(Bitboard board) { MoveList ml = {0}; return ml; }
MoveList generate_white_knight_moves(Bitboard board) { MoveList ml = {0}; return ml; }
MoveList generate_white_bishop_moves(Bitboard board) { MoveList ml = {0}; return ml; }
MoveList generate_white_rook_moves(Bitboard board) { MoveList ml = {0}; return ml; }
MoveList generate_white_queen_moves(Bitboard board) { MoveList ml = {0}; return ml; }
MoveList generate_white_king_moves(Bitboard board) { MoveList ml = {0}; return ml; }

MoveList generate_black_pawn_moves(Bitboard board) { MoveList ml = {0}; return ml; }
MoveList generate_black_knight_moves(Bitboard board) { MoveList ml = {0}; return ml; }
MoveList generate_black_bishop_moves(Bitboard board) { MoveList ml = {0}; return ml; }
MoveList generate_black_rook_moves(Bitboard board) { MoveList ml = {0}; return ml; }
MoveList generate_black_queen_moves(Bitboard board) { MoveList ml = {0}; return ml; }
MoveList generate_black_king_moves(Bitboard board) { MoveList ml = {0}; return ml; }

MoveList generate_moves(Bitboard board, int piece_type) {
    switch (piece_type) {
        case PIECE_WPAWN: return generate_white_pawn_moves(board);
        case PIECE_WKNIGHT: return generate_white_knight_moves(board);
        case PIECE_WBISHOP: return generate_white_bishop_moves(board);
        case PIECE_WROOK: return generate_white_rook_moves(board);
        case PIECE_WQUEEN: return generate_white_queen_moves(board);
        case PIECE_WKING: return generate_white_king_moves(board);
        case PIECE_BPAWN: return generate_black_pawn_moves(board);
        case PIECE_BKNIGHT: return generate_black_knight_moves(board);
        case PIECE_BBISHOP: return generate_black_bishop_moves(board);
        case PIECE_BROOK: return generate_black_rook_moves(board);
        case PIECE_BQUEEN: return generate_black_queen_moves(board);
        case PIECE_BKING: return generate_black_king_moves(board);
        default: { MoveList ml = {0}; return ml; }
    }
}

struct TreeNode {
	Move move;
    Bitboard state;
    struct TreeNode *parent;
    struct TreeNode **children;
	uint16_t score;
	uint8_t depth;
}TreeNode;

uint16_t evaluate_position( struct TreeNode *Node) {
    return 0;
}