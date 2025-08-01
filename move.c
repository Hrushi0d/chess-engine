//
// Created by lenovo on 7/31/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "constants.h"
#include "bitboard.h"
#include "move.h"

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

// Optimized occupancy update functions
static inline void update_occupancy_remove(Bitboard *b, uint64_t sq, uint8_t piece) {
    b->all_occupancy ^= sq;
    if (IS_BLACK(piece)) {
        b->black_occupancy ^= sq;
    } else {
        b->white_occupancy ^= sq;
    }
}

static inline void update_occupancy_add(Bitboard *b, uint64_t sq, uint8_t piece) {
    b->all_occupancy |= sq;
    if (IS_BLACK(piece)) {
        b->black_occupancy |= sq;
    } else {
        b->white_occupancy |= sq;
    }
}

// Precomputed castling masks for faster updates
static const uint8_t CASTLE_KING_LOST[2] = {
    ~(CASTLE_WHITE_K | CASTLE_WHITE_Q),
    ~(CASTLE_BLACK_K | CASTLE_BLACK_Q)
};

static uint8_t CASTLE_ROOK_LOST[64] = {
    [0 ... 63] = 0xFF,
    [0] = ~CASTLE_WHITE_Q,
    [7] = ~CASTLE_WHITE_K,
    [56] = ~CASTLE_BLACK_Q,
    [63] = ~CASTLE_BLACK_K,
};

// __attribute__((constructor))
// static void init_castle_lookup() {
//     for (int i = 0; i < 64; i++) {
//         if (i != 0 && i != 7 && i != 56 && i != 63) {
//             CASTLE_ROOK_LOST[i] = 0xFF;
//         }
//     }
// }

// --- Internal Helpers ---

static inline void remove_piece(Bitboard* b, uint64_t sq, uint8_t piece) {
    update_occupancy_remove(b, sq, piece);
    b->pieces[PIECE_INDEX(piece)] ^= sq;
}

static inline void add_piece(Bitboard* b, uint64_t sq, uint8_t piece) {
    update_occupancy_add(b, sq, piece);
    b->pieces[PIECE_INDEX(piece)] |= sq;
}

static void apply_castling(Bitboard* b, const Move* m) {
    uint64_t rook_from, rook_to;
    uint8_t rook_piece = MOVING ? PIECE_BROOK : PIECE_WROOK;

    switch (m->to_square) {
        case CASTLE_WK_TO: rook_from = 1ULL << 7;  rook_to = 1ULL << 5;  break;
        case CASTLE_WQ_TO: rook_from = 1ULL << 0;  rook_to = 1ULL << 3;  break;
        case CASTLE_BK_TO: rook_from = 1ULL << 63; rook_to = 1ULL << 61; break;
        case CASTLE_BQ_TO: rook_from = 1ULL << 56; rook_to = 1ULL << 59; break;
        default:
            fprintf(stderr, "Invalid castling target: %u\n", m->to_square);
            exit(EXIT_FAILURE);
    }

    remove_piece(b, rook_from, rook_piece);
    add_piece(b, rook_to, rook_piece);
    b->pieces[PIECE_INDEX(rook_piece)] ^= (rook_from | rook_to);
}

static void undo_castling(Bitboard* b, const Move* m) {
    uint64_t rook_from, rook_to;
    uint8_t rook_piece = MOVING ? PIECE_WROOK : PIECE_BROOK;

    switch (m->to_square) {
        case CASTLE_WK_TO: rook_from = 1ULL << 5;  rook_to = 1ULL << 7;  break;
        case CASTLE_WQ_TO: rook_from = 1ULL << 3;  rook_to = 1ULL << 0;  break;
        case CASTLE_BK_TO: rook_from = 1ULL << 61; rook_to = 1ULL << 63; break;
        case CASTLE_BQ_TO: rook_from = 1ULL << 59; rook_to = 1ULL << 56; break;
        default:
            fprintf(stderr, "Invalid castling target: %u\n", m->to_square);
            exit(EXIT_FAILURE);
    }

    remove_piece(b, rook_from, rook_piece);
    add_piece(b, rook_to, rook_piece);
    b->pieces[PIECE_INDEX(rook_piece)] ^= (rook_from | rook_to);
}

static void apply_en_passant(Bitboard* b, const Move* m) {
    uint8_t ep_rank = MOVING ? (m->to_square >> 3) - 1 : (m->to_square >> 3) + 1;
    uint8_t ep_file = m->to_square & 7;
    uint64_t ep_bit = 1ULL << (ep_rank * 8 + ep_file);
    remove_piece(b, ep_bit, m->captured);
}

static void undo_en_passant(Bitboard* b, const Move* m) {
    uint8_t ep_rank = MOVING ? (m->to_square >> 3) - 1 : (m->to_square >> 3) + 1;
    uint8_t ep_file = m->to_square & 7;
    uint64_t ep_bit = 1ULL << (ep_rank * 8 + ep_file);
    add_piece(b, ep_bit, m->captured);
}

static void update_en_passant_target(Bitboard* b, const Move* m) {
    b->en_passant_target = 0;
    b->en_passant_file = 0xFF;
    b->en_passant_rank = 0xFF;

    if ((m->piece & 0x0F) == PIECE_PAWN) {
        int from_rank = m->from_square >> 3;
        int to_rank   = m->to_square >> 3;
        int file      = m->to_square & 7;

        if (IS_WHITE_TURN && from_rank == 1 && to_rank == 3) {
            int ep_rank = 2;
            b->en_passant_target = 1ULL << (ep_rank * 8 + file);
            b->en_passant_rank = ep_rank;
            b->en_passant_file = file;
        } else if (IS_BLACK_TURN && from_rank == 6 && to_rank == 4) {
            int ep_rank = 5;
            b->en_passant_target = 1ULL << (ep_rank * 8 + file);
            b->en_passant_rank = ep_rank;
            b->en_passant_file = file;
        }
    }
}


static void update_castling_rights(Bitboard* b, const Move* m) {
    if ((m->piece & 0x0F) == PIECE_KING) {
        b->castling_rights &= CASTLE_KING_LOST[MOVING];
    } else if ((m->piece & 0x0F) == PIECE_ROOK) {
        b->castling_rights &= CASTLE_ROOK_LOST[m->from_square];
    }

    if (IS_CAPTURE(*m)) {
        b->castling_rights &= CASTLE_ROOK_LOST[m->to_square];
    }
}

// --- MakeMove & UndoMove ---

Bitboard MakeMove(Move m, Bitboard b) {
    Bitboard mb = b;

    remove_piece(&mb, m.from, m.piece);

    if (IS_PROMO(m)) {
        add_piece(&mb, m.to, m.promotion);
    } else {
        add_piece(&mb, m.to, m.piece);
    }

    if (IS_CAPTURE(m) && !IS_EP(m)) {
        remove_piece(&mb, m.to, m.captured);
    }

    if (IS_EP(m)) apply_en_passant(&mb, &m);
    if (IS_CASTLE(m)) apply_castling(&mb, &m);

    update_en_passant_target(&mb, &m);
    update_castling_rights(&mb, &m);

    mb.halfmove_clock = ((m.piece & 0x0F) == PIECE_PAWN || IS_CAPTURE(m)) ? 0 : mb.halfmove_clock + 1;
    if (mb.to_move == 1) mb.fullmove_number++;
    mb.to_move ^= 1;

    return mb;
}

Bitboard UndoMove(Move m, Bitboard b) {
    Bitboard mb = b;
    mb.to_move ^= 1;
    if (mb.to_move == 1) mb.fullmove_number--;

    if (IS_CASTLE(m)) undo_castling(&mb, &m);

    if (IS_PROMO(m)) {
        remove_piece(&mb, m.to, m.promotion);
        add_piece(&mb, m.from, m.piece);
    } else {
        remove_piece(&mb, m.to, m.piece);
        add_piece(&mb, m.from, m.piece);
    }

    if (IS_CAPTURE(m)) {
        if (IS_EP(m)) undo_en_passant(&mb, &m);
        else add_piece(&mb, m.to, m.captured);
    }

    mb.en_passant_target = m.prev_en_passant_target;
    mb.en_passant_rank = m.prev_en_passant_rank;
    mb.en_passant_file = m.prev_en_passant_file;
    mb.castling_rights = m.prev_castling_rights;
    mb.halfmove_clock = m.prev_halfmove_clock;

    return mb;
}