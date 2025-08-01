//
// Created by lenovo on 7/30/2025.
//

#ifndef BITBOARD_H
#define BITBOARD_H

// === Chess Piece values (bitboard index) ===
#define PIECE_PAWN     0
#define PIECE_KNIGHT   1
#define PIECE_BISHOP   2
#define PIECE_ROOK     3
#define PIECE_QUEEN    4
#define PIECE_KING     5

#define PIECE_WPAWN     PIECE_PAWN
#define PIECE_WKNIGHT   PIECE_KNIGHT
#define PIECE_WBISHOP   PIECE_BISHOP
#define PIECE_WROOK     PIECE_ROOK
#define PIECE_WQUEEN    PIECE_QUEEN
#define PIECE_WKING     PIECE_KING

#define PIECE_BPAWN     (PIECE_PAWN   | 16)
#define PIECE_BKNIGHT   (PIECE_KNIGHT | 16)
#define PIECE_BBISHOP   (PIECE_BISHOP | 16)
#define PIECE_BROOK     (PIECE_ROOK   | 16)
#define PIECE_BQUEEN    (PIECE_QUEEN  | 16)
#define PIECE_BKING     (PIECE_KING   | 16)

#define INDEX_WPAWN     0
#define INDEX_WKNIGHT   1
#define INDEX_WBISHOP   2
#define INDEX_WROOK     3
#define INDEX_WQUEEN    4
#define INDEX_WKING     5

#define INDEX_BPAWN     6
#define INDEX_BKNIGHT   7
#define INDEX_BBISHOP   8
#define INDEX_BROOK     9
#define INDEX_BQUEEN    10
#define INDEX_BKING     11

#define PIECE_EMPTY     -1  // Not used in bitboards but may help in logic
#define INDEX_EMPTY     -1

// === castling ===

#define CASTLE_WHITE_K 0x1
#define CASTLE_WHITE_Q 0x2
#define CASTLE_WHITE   0x3
#define CASTLE_BLACK_K 0x4
#define CASTLE_BLACK_Q 0x8
#define CASTLE_BLACK   0xC

// === enpasant ===

// === Move generation ===
#define MAX_MOVES              256
#define MOVE_FLAG_CAPTURE      0x01
#define MOVE_FLAG_PROMOTION    0x02
#define MOVE_FLAG_ENPASSANT    0x04
#define MOVE_FLAG_CASTLE       0x08

#endif // BITBOARD_H

