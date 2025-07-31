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

#define IS_BLACK(piece) ((piece) & 0xF0)
#define IS_WHITE(piece) (!IS_BLACK(piece))
#define IS_PROMOTION(m) ((m).flags & MOVE_FLAG_PROMOTION)
#define IS_CAPTURE(m) ((m).flags & MOVE_FLAG_CAPTURE)
#define IS_ENPASSANT(m) ((m).flags & MOVE_FLAG_ENPASSANT)
#define IS_CASTLE(m) ((m).flags & MOVE_FLAG_CASTLE)

#endif //MOVE_H
