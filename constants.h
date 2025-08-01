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

#define INDEX_PAWN     0
#define INDEX_KNIGHT   1
#define INDEX_BISHOP   2
#define INDEX_ROOK     3
#define INDEX_QUEEN    4
#define INDEX_KING     5

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

#define MASK_FILE_A  0x0101010101010101ULL  // File a: bits   0,  8, 16, ..., 56
#define MASK_FILE_B  0x0202020202020202ULL  // File b: bits   1,  9, 17, ..., 57
#define MASK_FILE_C  0x0404040404040404ULL  // File c: bits   2, 10, 18, ..., 58
#define MASK_FILE_D  0x0808080808080808ULL  // File d: bits   3, 11, 19, ..., 59
#define MASK_FILE_E  0x1010101010101010ULL  // File e: bits   4, 12, 20, ..., 60
#define MASK_FILE_F  0x2020202020202020ULL  // File f: bits   5, 13, 21, ..., 61
#define MASK_FILE_G  0x4040404040404040ULL  // File g: bits   6, 14, 22, ..., 62
#define MASK_FILE_H  0x8080808080808080ULL  // File h: bits   7, 15, 23, ..., 63

#define MASK_RANK_1  0x00000000000000FFULL  // bits  0–7   (rank 0 in 0-based indexing)
#define MASK_RANK_2  0x000000000000FF00ULL  // bits  8–15
#define MASK_RANK_3  0x0000000000FF0000ULL  // bits  16–23
#define MASK_RANK_4  0x00000000FF000000ULL  // bits  24–31
#define MASK_RANK_5  0x000000FF00000000ULL  // bits  32–39
#define MASK_RANK_6  0x0000FF0000000000ULL  // bits  40–47
#define MASK_RANK_7  0x00FF000000000000ULL  // bits  48–55
#define MASK_RANK_8  0xFF00000000000000ULL  // bits  56–63

static const uint64_t FILE_MASKS[8] = {
    MASK_FILE_A,
    MASK_FILE_B,
    MASK_FILE_C,
    MASK_FILE_D,
    MASK_FILE_E,
    MASK_FILE_F,
    MASK_FILE_G,
    MASK_FILE_H
};

static const uint64_t RANK_MASKS[8] = {
    MASK_RANK_1,
    MASK_RANK_2,
    MASK_RANK_3,
    MASK_RANK_4,
    MASK_RANK_5,
    MASK_RANK_6,
    MASK_RANK_7,
    MASK_RANK_8,
};

#ifndef WHITE
#define WHITE false
#endif

#ifndef BLACK
#define BLACK true
#endif

#endif // BITBOARD_H

