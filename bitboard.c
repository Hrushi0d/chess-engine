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



Bitboard initBitboard(const char* FEN) {
    Bitboard b = {0};
    uint8_t row = 0, col = 0;

    while (*FEN && row < 8) {
        char c = *FEN++;

        if (c == '/') {
            row++;
            col = 0;
        } else if (isdigit(c)) {
            col += c - '0';
        } else {
            if (col < 8) {
                uint8_t piece = piece_from_char(c);
                if (piece != PIECE_EMPTY) {
                    uint8_t index = index_from_piece(piece);
					if (index != INDEX_EMPTY) {
						uint64_t bit = square_bit(row, col);
                    	b.pieces[index] |= bit;  // Mask to get black
                	    if (piece & 0xF0)
                    	    b.black_occupancy |= bit;
                    	else // or white
                        	b.white_occupancy |= bit;
                    	b.all_occupancy |= bit;
					}else{
						fprintf(stderr, "Invalid piece '%c' at row %d, col %d\n", c, row, col);
						exit(EXIT_FAILURE);
					}

                }else{ //raise Error invalid Symbol
				fprintf(stderr, "Invalid piece '%c' at row %d, col %d\n", c, row, col);
				exit(EXIT_FAILURE);
				}
                col++;
            }
        }
    }

	if (*FEN == ' ') FEN++;
    if (*FEN == 'w') b.to_move = 0;
    else if (*FEN == 'b') b.to_move = 1;
    else {
        fprintf(stderr, "Invalid side to move in FEN.\n");
        exit(EXIT_FAILURE);
    }

	if (*++FEN != ' ') FEN++;  // skip side char and space
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

    return b;
}


