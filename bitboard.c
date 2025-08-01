#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "constants.h"
#include "utils.h"

// Bitboard representation
typedef struct {
    uint64_t pieces[12]; // 0-5: White (P,N,B,R,Q,K), 6-11: Black (p,n,b,r,q,k)
    uint64_t white_occupancy;
    uint64_t black_occupancy;
    uint64_t all_occupancy;
	bool to_move;              // 0 = white, 1 = black
    uint8_t castling_rights;  // KQkq bits
    uint64_t en_passant_target;  // 0-7 or 8 if none
	uint8_t en_passant_rank;
	uint8_t en_passant_file;
	uint16_t halfmove_clock;
    uint16_t fullmove_number;
} Bitboard;

// Piece mapping helper
int piece_from_char(char c) {
    switch (c) {
        case 'P': return PIECE_WPAWN;
        case 'N': return PIECE_WKNIGHT;
        case 'B': return PIECE_WBISHOP;
        case 'R': return PIECE_WROOK;
        case 'Q': return PIECE_WQUEEN;
        case 'K': return PIECE_WKING;

        case 'p': return PIECE_BPAWN;
        case 'n': return PIECE_BKNIGHT;
        case 'b': return PIECE_BBISHOP;
        case 'r': return PIECE_BROOK;
        case 'q': return PIECE_BQUEEN;
        case 'k': return PIECE_BKING;

        default: return PIECE_EMPTY;
    }
}

int index_from_piece(uint8_t piece) {
    switch (piece) {
        case PIECE_WPAWN:   return INDEX_WPAWN;
		case PIECE_WKNIGHT: return INDEX_WKNIGHT;
		case PIECE_WBISHOP: return INDEX_WBISHOP;
		case PIECE_WROOK:   return INDEX_WROOK;
		case PIECE_WQUEEN:  return INDEX_WQUEEN;
		case PIECE_WKING:   return INDEX_WKING;

		case PIECE_BPAWN:   return INDEX_BPAWN;
		case PIECE_BKNIGHT: return INDEX_BKNIGHT;
		case PIECE_BBISHOP: return INDEX_BBISHOP;
		case PIECE_BROOK:   return INDEX_BROOK;
		case PIECE_BQUEEN:  return INDEX_BQUEEN;
		case PIECE_BKING:   return INDEX_BKING;

		default: return INDEX_EMPTY;
    }
}



Bitboard init_Bitboard(const char* FEN) {
    Bitboard b = {0};
    uint8_t row = 0, col = 0;
    const char* original_FEN = FEN; // Keep track of original for debugging

    // printf("DEBUG: Starting FEN parsing: '%s'\n", FEN);

    // Parse piece placement
    while (*FEN && row < 8) {
        char c = *FEN;
        // printf("DEBUG: Processing char '%c' at row=%d, col=%d, remaining FEN: '%s'\n", c, row, col, FEN);
        FEN++;

        if (c == '/') {
            row++;
            col = 0;
            // printf("DEBUG: New row, now row=%d, col=%d\n", row, col);
        } else if (isdigit(c)) {
            int skip = c - '0';
            col += skip;
            // printf("DEBUG: Skipping %d squares, col now = %d\n", skip, col);
        } else {
            if (col < 8) {
                uint8_t piece = piece_from_char(c);
                if (piece != PIECE_EMPTY) {
                    uint8_t index = index_from_piece(piece);
                    if (index != INDEX_EMPTY) {
                        uint64_t bit = square_bit(row, col);
                        // printf("DEBUG: Placing piece '%c' (piece=%d, index=%d) at row=%d, col=%d, bit=0x%llx\n" c, piece, index, row, col, bit);
                        b.pieces[index] |= bit;
                        if (piece & 0x10) // Check bit 4 (0x10) for black pieces
                            b.black_occupancy |= bit;
                        else
                            b.white_occupancy |= bit;
                        b.all_occupancy |= bit;
                    } else {
                        fprintf(stderr, "Invalid piece index for '%c' at row %d, col %d\n", c, row, col);
                        exit(EXIT_FAILURE);
                    }
                } else {
                    fprintf(stderr, "Invalid piece '%c' at row %d, col %d\n", c, row, col);
                    exit(EXIT_FAILURE);
                }
                col++;
                // printf("DEBUG: After placing piece, col=%d\n", col);
            }
        }
        // Break out of loop when we've finished the last row
        if (row == 7 && col == 8) {
            break;
        }
    }

    // printf("DEBUG: Finished piece placement. Current FEN position: '%s'\n", FEN);
    // printf("DEBUG: Next char is '%c' (ASCII: %d)\n", *FEN ? *FEN : '?', *FEN);

    // Skip whitespace before side to move
    while (*FEN && *FEN == ' ') {
        FEN++;
    }

    // printf("DEBUG: After skipping whitespace, next char is '%c' (ASCII: %d)\n", *FEN ? *FEN : '?', *FEN);

    // Parse side to move
    if (*FEN == 'w') {
        b.to_move = 0;
        // printf("DEBUG: Side to move: White\n");
    } else if (*FEN == 'b') {
        b.to_move = 1;
        // printf("DEBUG: Side to move: Black\n");
    } else {
        fprintf(stderr, "Invalid side to move in FEN: '%c' (ASCII: %d)\n", *FEN, (int)*FEN);
        fprintf(stderr, "Original FEN: '%s'\n", original_FEN);
        fprintf(stderr, "Current position in FEN: '%s'\n", FEN);
        exit(EXIT_FAILURE);
    }
    FEN++; // move past 'w' or 'b'

    // Skip whitespace before castling rights
    while (*FEN && *FEN == ' ') {
        FEN++;
    }

    // Parse castling rights
    b.castling_rights = 0;
    if (*FEN == '-') {
        FEN++;
    } else {
        while (*FEN && *FEN != ' ') {
            switch (*FEN) {
                case 'K': b.castling_rights |= CASTLE_WHITE_K; break;
                case 'Q': b.castling_rights |= CASTLE_WHITE_Q; break;
                case 'k': b.castling_rights |= CASTLE_BLACK_K; break;
                case 'q': b.castling_rights |= CASTLE_BLACK_Q; break;
                default:
                    fprintf(stderr, "Invalid castling character: %c\n", *FEN);
                    exit(EXIT_FAILURE);
            }
            FEN++;
        }
    }

    // For now, just set defaults for the remaining fields
    b.en_passant_target = 0;
    b.en_passant_file = 0xFF;
    b.en_passant_rank = 0xFF;
    b.halfmove_clock = 0;
    b.fullmove_number = 1;

    return b;
}

static const char piece_symbols[12] = {
    'P', 'N', 'B', 'R', 'Q', 'K',  // White
    'p', 'n', 'b', 'r', 'q', 'k'   // Black
};

void print_board(const Bitboard board) {
    for (int rank = 7; rank >= 0; rank--) {
        printf("%d ", rank + 1);
        for (int file = 0; file < 8; file++) {
            int sq = rank * 8 + file;
            uint64_t mask = 1ULL << sq;
            char piece = '.';

            for (int i = 0; i < 12; i++) {
                if (board.pieces[i] & mask) {
                    piece = piece_symbols[i];
                    break;
                }
            }

            printf("%c ", piece);
        }
        printf("\n");
    }

    printf("  a b c d e f g h\n");

    // Metadata
    printf("To move: %s\n", board.to_move ? "Black" : "White");
    printf("Castling rights: 0x%X\n", board.castling_rights);
    printf("En passant target: 0x%llx\n", board.en_passant_target);
    printf("Halfmove clock: %u\n", board.halfmove_clock);
    printf("Fullmove number: %u\n", board.fullmove_number);
}
