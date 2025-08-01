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
    ~(CASTLE_WHITE_K | CASTLE_WHITE_Q),  // White king moves
    ~(CASTLE_BLACK_K | CASTLE_BLACK_Q)   // Black king moves
};

// Initialize castling lookup table at compile time with proper defaults
static uint8_t CASTLE_ROOK_LOST[64] = {
    [0 ... 63] = 0xFF,           // Default: no castling rights lost
    [0] = ~CASTLE_WHITE_Q,       // a1 rook
    [7] = ~CASTLE_WHITE_K,       // h1 rook
    [56] = ~CASTLE_BLACK_Q,      // a8 rook
    [63] = ~CASTLE_BLACK_K,      // h8 rook
};

// Alternative: Initialize at program startup using constructor attribute
// This runs once before main(), eliminating any runtime overhead
__attribute__((constructor))
static void init_castle_lookup() {
    // Ensure all squares default to no castling rights lost
    for (int i = 0; i < 64; i++) {
        if (i != 0 && i != 7 && i != 56 && i != 63) {
            CASTLE_ROOK_LOST[i] = 0xFF;
        }
    }
}

Bitboard MakeMove(Move m, Bitboard b) {
    Bitboard mb = b;

    // Remove piece from source square
    update_occupancy_remove(&mb, m.from, m.piece);
    mb.pieces[PIECE_INDEX(m.piece)] ^= m.from;

    // Handle promotion vs normal move
    if (IS_PROMO(m)) {
        // Place promoted piece on target square
        update_occupancy_add(&mb, m.to, m.promotion);
        mb.pieces[PIECE_INDEX(m.promotion)] |= m.to;
    } else {
        // Place same piece on target square
        update_occupancy_add(&mb, m.to, m.piece);
        mb.pieces[PIECE_INDEX(m.piece)] |= m.to;
    }

    // Handle captures (except en passant)
    if (IS_CAPTURE(m) && !IS_EP(m)) {
        // Captured piece is already removed from occupancy by the move above
        // Just need to remove from piece bitboard
        mb.pieces[PIECE_INDEX(m.captured)] ^= m.to;
    }

    // Handle en passant capture
    if (IS_EP(m)) {
        // Calculate captured pawn position
        uint8_t ep_rank = IS_BLACK(m.piece) ? (m.to_square >> 3) + 1 : (m.to_square >> 3) - 1;
        uint8_t ep_file = m.to_square & 7;
        uint8_t ep_square = ep_rank * 8 + ep_file;
        uint64_t ep_bit = 1ULL << ep_square;

        update_occupancy_remove(&mb, ep_bit, m.captured);
        mb.pieces[PIECE_INDEX(m.captured)] ^= ep_bit;
    }

    // Handle castling
    if (IS_CASTLE(m)) {
        uint64_t rook_from, rook_to;
        uint8_t rook_piece = IS_BLACK(m.piece) ? PIECE_BROOK : PIECE_WROOK;

        // Use lookup table for rook positions
        switch (m.to_square) {
            case CASTLE_WK_TO: rook_from = 1ULL << 7; rook_to = 1ULL << 5; break;  // h1->f1
            case CASTLE_WQ_TO: rook_from = 1ULL << 0; rook_to = 1ULL << 3; break;  // a1->d1
            case CASTLE_BK_TO: rook_from = 1ULL << 63; rook_to = 1ULL << 61; break; // h8->f8
            case CASTLE_BQ_TO: rook_from = 1ULL << 56; rook_to = 1ULL << 59; break; // a8->d8
            default:
                fprintf(stderr, "Invalid castling target square: %u\n", m.to_square);
                exit(EXIT_FAILURE);
        }

        // Move the rook
        update_occupancy_remove(&mb, rook_from, rook_piece);
        update_occupancy_add(&mb, rook_to, rook_piece);
        mb.pieces[PIECE_INDEX(rook_piece)] ^= (rook_from | rook_to);
    }

    // Clear en passant (always reset)
    mb.en_passant_target = 0;
    mb.en_passant_file = 0xFF;
    mb.en_passant_rank = 0xFF;

    // Set new en passant if pawn double move
    if ((m.piece & 0x0F) == PIECE_PAWN) {
        uint8_t from_rank = m.from_square >> 3;
        uint8_t to_rank = m.to_square >> 3;

        if (abs((int)to_rank - (int)from_rank) == 2) {
            uint8_t file = m.to_square & 7;
            uint8_t ep_rank = (from_rank + to_rank) >> 1; // Average rank
            mb.en_passant_target = 1ULL << (ep_rank * 8 + file);
            mb.en_passant_file = file;
            mb.en_passant_rank = ep_rank;
        }
    }

    // Update castling rights efficiently
    if ((m.piece & 0x0F) == PIECE_KING) {
        // King moved - lose all castling rights for this color
        mb.castling_rights &= CASTLE_KING_LOST[IS_BLACK(m.piece) ? 1 : 0];
    } else if ((m.piece & 0x0F) == PIECE_ROOK) {
        // Rook moved - potentially lose castling rights
        mb.castling_rights &= CASTLE_ROOK_LOST[m.from_square];
    }

    // Handle captures affecting castling rights
    if (IS_CAPTURE(m)) {
        mb.castling_rights &= CASTLE_ROOK_LOST[m.to_square];
    }

    // Update halfmove clock and move counters
    mb.halfmove_clock = ((m.piece & 0x0F) == PIECE_PAWN || IS_CAPTURE(m)) ? 0 : mb.halfmove_clock + 1;

    // Increment fullmove number after black's move
    if (mb.to_move == 1) {
        mb.fullmove_number++;
    }

    // Switch sides
    mb.to_move ^= 1;

    return mb;
}

Bitboard UndoMove(Move m, Bitboard board) {
    Bitboard mb = board;

    // Switch sides back
    mb.to_move ^= 1;

    // Decrement fullmove number if it was black's move
    if (mb.to_move == 1) {
        mb.fullmove_number--;
    }

    // Undo castling first (if applicable)
    if (IS_CASTLE(m)) {
        uint64_t rook_from, rook_to; // Note: these are swapped for undo
        uint8_t rook_piece = IS_BLACK(m.piece) ? PIECE_BROOK : PIECE_WROOK;

        switch (m.to_square) {
            case CASTLE_WK_TO: rook_from = 1ULL << 5; rook_to = 1ULL << 7; break;   // f1->h1
            case CASTLE_WQ_TO: rook_from = 1ULL << 3; rook_to = 1ULL << 0; break;   // d1->a1
            case CASTLE_BK_TO: rook_from = 1ULL << 61; rook_to = 1ULL << 63; break; // f8->h8
            case CASTLE_BQ_TO: rook_from = 1ULL << 59; rook_to = 1ULL << 56; break; // d8->a8
            default:
                fprintf(stderr, "Invalid castling target square: %u\n", m.to_square);
                exit(EXIT_FAILURE);
        }

        // Move rook back
        update_occupancy_remove(&mb, rook_from, rook_piece);
        update_occupancy_add(&mb, rook_to, rook_piece);
        mb.pieces[PIECE_INDEX(rook_piece)] ^= (rook_from | rook_to);
    }

    // Move piece back from target to source
    if (IS_PROMO(m)) {
        // Remove promoted piece from target square
        update_occupancy_remove(&mb, m.to, m.promotion);
        mb.pieces[PIECE_INDEX(m.promotion)] ^= m.to;

        // Restore original pawn to source square
        update_occupancy_add(&mb, m.from, m.piece);
        mb.pieces[PIECE_INDEX(m.piece)] |= m.from;
    } else {
        // Move piece back normally
        update_occupancy_remove(&mb, m.to, m.piece);
        update_occupancy_add(&mb, m.from, m.piece);
        mb.pieces[PIECE_INDEX(m.piece)] ^= (m.from | m.to);
    }

    // Restore captured piece (if any)
    if (IS_CAPTURE(m)) {
        if (IS_EP(m)) {
            // Restore en passant captured pawn
            uint8_t ep_rank = IS_BLACK(m.piece) ? (m.to_square >> 3) + 1 : (m.to_square >> 3) - 1;
            uint8_t ep_file = m.to_square & 7;
            uint8_t ep_square = ep_rank * 8 + ep_file;
            uint64_t ep_bit = 1ULL << ep_square;

            update_occupancy_add(&mb, ep_bit, m.captured);
            mb.pieces[PIECE_INDEX(m.captured)] |= ep_bit;
        } else {
            // Restore normally captured piece
            update_occupancy_add(&mb, m.to, m.captured);
            mb.pieces[PIECE_INDEX(m.captured)] |= m.to;
        }
    }

    // Restore previous board state
    mb.en_passant_target = m.prev_en_passant_target;
    mb.en_passant_rank = m.prev_en_passant_rank;
    mb.en_passant_file = m.prev_en_passant_file;
    mb.castling_rights = m.prev_castling_rights;
    mb.halfmove_clock = m.prev_halfmove_clock;

    return mb;
}