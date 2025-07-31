//
// Created by lenovo on 7/31/2025.
//

#ifndef BITBOARD_H
#define BITBOARD_H

typedef struct {
    uint64_t pieces[12];        // 0-5: White (P,N,B,R,Q,K), 6-11: Black (p,n,b,r,q,k)
    uint64_t white_occupancy;
    uint64_t black_occupancy;
    uint64_t all_occupancy;
    bool to_move;               // 0 = white, 1 = black
    uint8_t castling_rights;    // KQkq bits
    uint64_t en_passant_target;
    uint8_t en_passant_rank;
    uint8_t en_passant_file;
    uint16_t halfmove_clock;
    uint16_t fullmove_number;
} Bitboard;

// === Function declarations ===
int piece_from_char(char c);
int index_from_piece(uint8_t piece);
uint64_t square_bit(int row, int col);
Bitboard initBitboard(const char* FEN);

#endif //BITBOARD_H
