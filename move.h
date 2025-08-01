//
// Created by lenovo on 7/31/2025.
//

#ifndef MOVE_H
#define MOVE_H

typedef struct {
    uint64_t from;              // bitboard representation of from square
    uint64_t to;                // bitboard representation of to square
    uint8_t from_square;        // square index (0-63)
    uint8_t to_square;          // square index (0-63)
    uint8_t piece;              // piece type being moved
    uint8_t captured;           // captured piece (if any)
    uint8_t promotion;          // promotion piece (if any)
    uint8_t flags;              // move flags

    // Store previous board state for undo
    uint64_t prev_en_passant_target;
    uint8_t prev_en_passant_rank;
    uint8_t prev_en_passant_file;
    uint8_t prev_castling_rights;
    uint16_t prev_halfmove_clock;
} Move;

Bitboard MakeMove(Move m, Bitboard board);
Bitboard UndoMove(Move m, Bitboard board);

#define IS_BLACK(piece) ((piece) & 0x10)
#define IS_WHITE(piece) (!IS_BLACK(piece))
#define PIECE_INDEX(p) index_from_piece(p)
#define SQUARE_FROM(m) (m.from)
#define SQUARE_TO(m)   (m.to)
#define IS_PROMO(m)    ((m.flags) & MOVE_FLAG_PROMOTION)
#define IS_CAPTURE(m)  ((m.flags) & MOVE_FLAG_CAPTURE)
#define IS_EP(m)       ((m.flags) & MOVE_FLAG_ENPASSANT)
#define IS_CASTLE(m)   ((m.flags) & MOVE_FLAG_CASTLE)

#define CASTLE_WK_TO 6   // g1
#define CASTLE_WQ_TO 2   // c1
#define CASTLE_BK_TO 62  // g8
#define CASTLE_BQ_TO 58  // c8

#endif //MOVE_H
