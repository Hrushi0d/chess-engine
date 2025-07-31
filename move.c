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

static inline void xor_occupancy(Bitboard *mb, uint64_t sq, uint8_t piece) {
    mb->all_occupancy ^= sq;
    if (IS_BLACK(piece)) mb->black_occupancy ^= sq;
    else mb->white_occupancy ^= sq;
}

static inline void or_occupancy(Bitboard *mb, uint64_t sq, uint8_t piece) {
    mb->all_occupancy |= sq;
    if (IS_BLACK(piece)) mb->black_occupancy |= sq;
    else mb->white_occupancy |= sq;
}

static inline void clear_en_passant(Bitboard *mb) {
    mb->en_passant_target = 0;
    mb->en_passant_file = 0xFF;
    mb->en_passant_rank = 0xFF;
}

static inline void restore_en_passant(Bitboard *mb, Move m) {
    mb->en_passant_target = m.prev_en_passant_target;
    mb->en_passant_rank = m.prev_en_passant_rank;
    mb->en_passant_file = m.prev_en_passant_file;
}

static inline void restore_castling(Bitboard *mb, Move m) {
    mb->castling_rights = m.prev_castling_rights;
}

static inline void restore_halfmove_clock(Bitboard *mb, Move m) {
    mb->halfmove_clock = m.prev_halfmove_clock;
}

Bitboard MakeMove(Move m, Bitboard b) {
    Bitboard mb = b;

    xor_occupancy(&mb, m.from, m.piece);
    mb.pieces[PIECE_INDEX(m.piece)] ^= m.from;

    if (IS_PROMO(m)) {
        or_occupancy(&mb, m.to, m.promotion);
        mb.pieces[PIECE_INDEX(m.promotion)] |= m.to;
    } else {
        or_occupancy(&mb, m.to, m.piece);
        mb.pieces[PIECE_INDEX(m.piece)] |= m.to;
    }

    if (IS_CAPTURE(m) && !IS_EP(m)) {
        xor_occupancy(&mb, m.to, m.captured);
        mb.pieces[PIECE_INDEX(m.captured)] ^= m.to;
    }

    if (IS_EP(m)) {
        int rank = __builtin_ctzll(m.to) / 8;
        int file = __builtin_ctzll(m.to) % 8;
        int cap_rank = IS_BLACK(m.piece) ? rank + 1 : rank - 1;
        uint64_t ep_sq = 1ULL << (cap_rank * 8 + file);
        xor_occupancy(&mb, ep_sq, m.captured);
        mb.pieces[PIECE_INDEX(m.captured)] ^= ep_sq;
    }

    if (IS_CASTLE(m)) {
        uint64_t r_from, r_to;
        uint8_t rook = (IS_BLACK(m.piece) ? PIECE_BROOK : PIECE_WROOK);
        switch (m.to_square) {
            case CASTLE_WK_TO: r_from = 1ULL << 7; r_to = 1ULL << 5; break;
            case CASTLE_WQ_TO: r_from = 1ULL << 0; r_to = 1ULL << 3; break;
            case CASTLE_BK_TO: r_from = 1ULL << 63; r_to = 1ULL << 61; break;
            case CASTLE_BQ_TO: r_from = 1ULL << 56; r_to = 1ULL << 59; break;
            default:
                fprintf(stderr, "Invalid castling square: %u\n", m.to_square);
                exit(EXIT_FAILURE);
        }
        xor_occupancy(&mb, r_from, rook);
        or_occupancy(&mb, r_to, rook);
        mb.pieces[PIECE_INDEX(rook)] ^= r_from;
        mb.pieces[PIECE_INDEX(rook)] |= r_to;
    }

    clear_en_passant(&mb);
    if ((m.piece & 0x0F) == PIECE_PAWN) {
        int rf = __builtin_ctzll(m.from) / 8, rt = __builtin_ctzll(m.to) / 8;
        if (abs(rt - rf) == 2) {
            int file = __builtin_ctzll(m.to) % 8;
            int ep_rank = (rf + rt) / 2;
            mb.en_passant_target = 1ULL << (ep_rank * 8 + file);
            mb.en_passant_file = file;
            mb.en_passant_rank = ep_rank;
        }
    }

    if (m.piece == PIECE_WKING) mb.castling_rights &= ~(CASTLE_WHITE_K | CASTLE_WHITE_Q);
    else if (m.piece == PIECE_BKING) mb.castling_rights &= ~(CASTLE_BLACK_K | CASTLE_BLACK_Q);
    else if (m.piece == PIECE_WROOK) {
        if (m.from_square == 0) mb.castling_rights &= ~CASTLE_WHITE_Q;
        if (m.from_square == 7) mb.castling_rights &= ~CASTLE_WHITE_K;
    } else if (m.piece == PIECE_BROOK) {
        if (m.from_square == 56) mb.castling_rights &= ~CASTLE_BLACK_Q;
        if (m.from_square == 63) mb.castling_rights &= ~CASTLE_BLACK_K;
    }
    if (IS_CAPTURE(m)) {
        if (m.to_square == 0) mb.castling_rights &= ~CASTLE_WHITE_Q;
        if (m.to_square == 7) mb.castling_rights &= ~CASTLE_WHITE_K;
        if (m.to_square == 56) mb.castling_rights &= ~CASTLE_BLACK_Q;
        if (m.to_square == 63) mb.castling_rights &= ~CASTLE_BLACK_K;
    }

    mb.halfmove_clock = ((m.piece & 0x0F) == PIECE_PAWN || IS_CAPTURE(m)) ? 0 : mb.halfmove_clock + 1;
    if (mb.to_move == 1) mb.fullmove_number++;
    mb.to_move ^= 1;
    return mb;
}

Bitboard UndoMove(Move m, Bitboard board) {
    Bitboard mb = board;
    mb.to_move ^= 1;
    if (mb.to_move == 1) mb.fullmove_number--;

    if (IS_CASTLE(m)) {
        uint64_t rf, rt;
        uint8_t rook = (IS_BLACK(m.piece) ? PIECE_BROOK : PIECE_WROOK);
        switch (m.to_square) {
            case CASTLE_WK_TO: rf = 1ULL << 5; rt = 1ULL << 7; break;
            case CASTLE_WQ_TO: rf = 1ULL << 3; rt = 1ULL << 0; break;
            case CASTLE_BK_TO: rf = 1ULL << 61; rt = 1ULL << 63; break;
            case CASTLE_BQ_TO: rf = 1ULL << 59; rt = 1ULL << 56; break;
            default:
                fprintf(stderr, "Invalid castling square: %u\n", m.to_square);
                exit(EXIT_FAILURE);
        }
        xor_occupancy(&mb, rf, rook);
        or_occupancy(&mb, rt, rook);
        mb.pieces[PIECE_INDEX(rook)] ^= rf;
        mb.pieces[PIECE_INDEX(rook)] |= rt;
    }

    xor_occupancy(&mb, m.to, m.piece);
    or_occupancy(&mb, m.from, m.piece);

    if (IS_PROMO(m)) {
        mb.pieces[PIECE_INDEX(m.promotion)] ^= m.to;
        mb.pieces[PIECE_INDEX(m.piece)] |= m.from;
    } else {
        mb.pieces[PIECE_INDEX(m.piece)] ^= m.to;
        mb.pieces[PIECE_INDEX(m.piece)] |= m.from;
    }

    if (IS_CAPTURE(m) && !IS_EP(m)) {
        mb.pieces[PIECE_INDEX(m.captured)] |= m.to;
        or_occupancy(&mb, m.to, m.captured);
    }
    if (IS_EP(m)) {
        int r = __builtin_ctzll(m.to) / 8;
        int f = __builtin_ctzll(m.to) % 8;
        int cap_r = IS_BLACK(m.piece) ? r + 1 : r - 1;
        uint64_t ep_sq = 1ULL << (cap_r * 8 + f);
        mb.pieces[PIECE_INDEX(m.captured)] |= ep_sq;
        or_occupancy(&mb, ep_sq, m.captured);
    }

    restore_en_passant(&mb, m);
    restore_castling(&mb, m);
    restore_halfmove_clock(&mb, m);

    return mb;
}
