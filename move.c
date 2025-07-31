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

Bitboard MakeMove(Move m, Bitboard board) {
    Bitboard mb = board;

    // Remove piece from 'from' square
    mb.all_occupancy ^= m.from;
    if (m.piece & 0x10) // black (fixed: was 0xF0, should be 0x10)
        mb.black_occupancy ^= m.from;
    else
        mb.white_occupancy ^= m.from;

    uint8_t from_index = index_from_piece(m.piece);
    mb.pieces[from_index] ^= m.from;

    // Handle promotion
    if (m.flags & MOVE_FLAG_PROMOTION) {
        // Place promoted piece on 'to' square
        mb.all_occupancy |= m.to;
        if (m.piece & 0x10)
            mb.black_occupancy |= m.to;
        else
            mb.white_occupancy |= m.to;

        uint8_t promotion_index = index_from_piece(m.promotion);
        mb.pieces[promotion_index] |= m.to;
    } else {
        // Move piece to 'to' square
        mb.all_occupancy |= m.to;
        if (m.piece & 0x10)
            mb.black_occupancy |= m.to;
        else
            mb.white_occupancy |= m.to;

        mb.pieces[from_index] |= m.to;
    }

    // Handle captures (but not en passant)
    if ((m.flags & MOVE_FLAG_CAPTURE) && !(m.flags & MOVE_FLAG_ENPASSANT)) {
        uint8_t captured_index = index_from_piece(m.captured);
        mb.pieces[captured_index] ^= m.to;

        if (m.captured & 0x10)
            mb.black_occupancy ^= m.to;
        else
            mb.white_occupancy ^= m.to;

        mb.all_occupancy ^= m.to;
    }

    // Handle en passant capture
    if (m.flags & MOVE_FLAG_ENPASSANT) {
        // Calculate the square where the captured pawn is located
        int rank_to = __builtin_ctzll(m.to) / 8;
        int file_to = __builtin_ctzll(m.to) % 8;
        int captured_rank = (m.piece & 0x10) ? rank_to + 1 : rank_to - 1;
        uint64_t ep_square = 1ULL << (captured_rank * 8 + file_to);

        uint8_t captured_index = index_from_piece(m.captured);
        mb.pieces[captured_index] ^= ep_square;

        if (m.captured & 0x10)
            mb.black_occupancy ^= ep_square;
        else
            mb.white_occupancy ^= ep_square;

        mb.all_occupancy ^= ep_square;
    }

    // Handle castling - FIXED LOGIC
    if (m.flags & MOVE_FLAG_CASTLE) {
        uint64_t rook_from, rook_to;
        uint8_t rook_piece = (m.piece & 0x10) | PIECE_ROOK;

        if (m.to_square == 6) { // White kingside (e1->g1)
            rook_from = 1ULL << 7;  // h1
            rook_to = 1ULL << 5;    // f1
        } else if (m.to_square == 2) { // White queenside (e1->c1)
            rook_from = 1ULL << 0;  // a1
            rook_to = 1ULL << 3;    // d1
        } else if (m.to_square == 62) { // Black kingside (e8->g8)
            rook_from = 1ULL << 63; // h8
            rook_to = 1ULL << 61;   // f8
        } else if (m.to_square == 58) { // Black queenside (e8->c8)
            rook_from = 1ULL << 56; // a8
            rook_to = 1ULL << 59;   // d8
        }

        // Move the rook
        mb.all_occupancy ^= rook_from;
        mb.all_occupancy |= rook_to;

        if (m.piece & 0x10) {
            mb.black_occupancy ^= rook_from;
            mb.black_occupancy |= rook_to;
        } else {
            mb.white_occupancy ^= rook_from;
            mb.white_occupancy |= rook_to;
        }

        uint8_t rook_index = index_from_piece(rook_piece);
        mb.pieces[rook_index] ^= rook_from;
        mb.pieces[rook_index] |= rook_to;
    }

    // Clear en passant square unless a double pawn push just happened
    mb.en_passant_target = 0;
    mb.en_passant_rank = 0xFF;
    mb.en_passant_file = 0xFF;

    // Handle setting en passant square if the move was a double pawn push
    if ((m.piece & 0x0F) == PIECE_PAWN) {
        int rank_from = __builtin_ctzll(m.from) / 8;
        int rank_to = __builtin_ctzll(m.to) / 8;
        if (abs(rank_to - rank_from) == 2) {
            int file = __builtin_ctzll(m.to) % 8;
            int ep_rank = (rank_from + rank_to) / 2;
            mb.en_passant_target = 1ULL << (ep_rank * 8 + file);
            mb.en_passant_file = file;
            mb.en_passant_rank = ep_rank;
        }
    }

    // Update castling rights
    if (m.piece == PIECE_WKING) {
        mb.castling_rights &= ~(CASTLE_WHITE_K | CASTLE_WHITE_Q);
    } else if (m.piece == PIECE_BKING) {
        mb.castling_rights &= ~(CASTLE_BLACK_K | CASTLE_BLACK_Q);
    } else if (m.piece == PIECE_WROOK) {
        if (m.from_square == 0) mb.castling_rights &= ~CASTLE_WHITE_Q;  // a1
        if (m.from_square == 7) mb.castling_rights &= ~CASTLE_WHITE_K;  // h1
    } else if (m.piece == PIECE_BROOK) {
        if (m.from_square == 56) mb.castling_rights &= ~CASTLE_BLACK_Q; // a8
        if (m.from_square == 63) mb.castling_rights &= ~CASTLE_BLACK_K; // h8
    }

    // Also check if rook was captured
    if (m.flags & MOVE_FLAG_CAPTURE) {
        if (m.to_square == 0) mb.castling_rights &= ~CASTLE_WHITE_Q;
        if (m.to_square == 7) mb.castling_rights &= ~CASTLE_WHITE_K;
        if (m.to_square == 56) mb.castling_rights &= ~CASTLE_BLACK_Q;
        if (m.to_square == 63) mb.castling_rights &= ~CASTLE_BLACK_K;
    }

    // Update halfmove clock
    if ((m.piece & 0x0F) == PIECE_PAWN || (m.flags & MOVE_FLAG_CAPTURE)) {
        mb.halfmove_clock = 0;  // Reset on pawn move or capture
    } else {
        mb.halfmove_clock++;
    }

    // Update fullmove number (increment after black's move)
    if (mb.to_move == 1) { // was black's turn
        mb.fullmove_number++;
    }

    // Switch turns
    mb.to_move = !mb.to_move;

    return mb;
}

Bitboard UndoMove(Move m, Bitboard board) {
    Bitboard mb = board;

    // Switch turns back
    mb.to_move = !mb.to_move;

    // Restore fullmove number
    if (mb.to_move == 1) { // now black's turn, so we undid black's move
        mb.fullmove_number--;
    }

    // Handle castling undo first - FIXED LOGIC
    if (m.flags & MOVE_FLAG_CASTLE) {
        uint64_t rook_from, rook_to;
        uint8_t rook_piece = (m.piece & 0x10) | PIECE_ROOK;

        if (m.to_square == 6) { // White kingside undo
            rook_from = 1ULL << 5;  // f1 (where rook is now)
            rook_to = 1ULL << 7;    // h1 (where rook should go back)
        } else if (m.to_square == 2) { // White queenside undo
            rook_from = 1ULL << 3;  // d1
            rook_to = 1ULL << 0;    // a1
        } else if (m.to_square == 62) { // Black kingside undo
            rook_from = 1ULL << 61; // f8
            rook_to = 1ULL << 63;   // h8
        } else if (m.to_square == 58) { // Black queenside undo
            rook_from = 1ULL << 59; // d8
            rook_to = 1ULL << 56;   // a8
        }

        // Undo rook movement
        mb.all_occupancy ^= rook_from;
        mb.all_occupancy |= rook_to;

        if (m.piece & 0x10) {
            mb.black_occupancy ^= rook_from;
            mb.black_occupancy |= rook_to;
        } else {
            mb.white_occupancy ^= rook_from;
            mb.white_occupancy |= rook_to;
        }

        uint8_t rook_index = index_from_piece(rook_piece);
        mb.pieces[rook_index] ^= rook_from;
        mb.pieces[rook_index] |= rook_to;
    }

    // Remove piece from 'to' square
    mb.all_occupancy ^= m.to;
    if (m.piece & 0x10)
        mb.black_occupancy ^= m.to;
    else
        mb.white_occupancy ^= m.to;

    // Place piece back on 'from' square
    mb.all_occupancy |= m.from;
    if (m.piece & 0x10)
        mb.black_occupancy |= m.from;
    else
        mb.white_occupancy |= m.from;

    // Handle piece restoration (undo promotion if needed)
    if (m.flags & MOVE_FLAG_PROMOTION) {
        // Remove promoted piece from board
        uint8_t promotion_index = index_from_piece(m.promotion);
        mb.pieces[promotion_index] ^= m.to;

        // Restore original pawn
        uint8_t pawn_index = index_from_piece(m.piece);
        mb.pieces[pawn_index] |= m.from;
    } else {
        uint8_t from_index = index_from_piece(m.piece);
        mb.pieces[from_index] ^= m.to;
        mb.pieces[from_index] |= m.from;
    }

    // Restore captured piece (regular capture)
    if ((m.flags & MOVE_FLAG_CAPTURE) && !(m.flags & MOVE_FLAG_ENPASSANT)) {
        uint8_t captured_index = index_from_piece(m.captured);
        mb.pieces[captured_index] |= m.to;

        if (m.captured & 0x10)
            mb.black_occupancy |= m.to;
        else
            mb.white_occupancy |= m.to;

        mb.all_occupancy |= m.to;
    }

    // Restore captured pawn from en passant
    if (m.flags & MOVE_FLAG_ENPASSANT) {
        int rank_to = __builtin_ctzll(m.to) / 8;
        int file_to = __builtin_ctzll(m.to) % 8;
        int captured_rank = (m.piece & 0x10) ? rank_to + 1 : rank_to - 1;
        uint64_t ep_square = 1ULL << (captured_rank * 8 + file_to);

        uint8_t captured_index = index_from_piece(m.captured);
        mb.pieces[captured_index] |= ep_square;

        if (m.captured & 0x10)
            mb.black_occupancy |= ep_square;
        else
            mb.white_occupancy |= ep_square;

        mb.all_occupancy |= ep_square;
    }

    // Restore previous board state
    mb.en_passant_target = m.prev_en_passant_target;
    mb.en_passant_rank = m.prev_en_passant_rank;
    mb.en_passant_file = m.prev_en_passant_file;
    mb.castling_rights = m.prev_castling_rights;
    mb.halfmove_clock = m.prev_halfmove_clock;

    return mb;
}