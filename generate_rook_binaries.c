//
// Created by lenovo on 8/4/2025.
//


//
// Created by lenovo on 8/4/2025.
//
#define MASK_FILE_A  0x0101010101010101ULL
#define MASK_FILE_B  0x0202020202020202ULL
#define MASK_FILE_C  0x0404040404040404ULL
#define MASK_FILE_D  0x0808080808080808ULL
#define MASK_FILE_E  0x1010101010101010ULL
#define MASK_FILE_F  0x2020202020202020ULL
#define MASK_FILE_G  0x4040404040404040ULL
#define MASK_FILE_H  0x8080808080808080ULL

#define MASK_RANK_1  0x00000000000000FFULL
#define MASK_RANK_2  0x000000000000FF00ULL
#define MASK_RANK_3  0x0000000000FF0000ULL
#define MASK_RANK_4  0x00000000FF000000ULL
#define MASK_RANK_5  0x000000FF00000000ULL
#define MASK_RANK_6  0x0000FF0000000000ULL
#define MASK_RANK_7  0x00FF000000000000ULL
#define MASK_RANK_8  0xFF00000000000000ULL

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "magic.h"


#define RANK_MASK(r) (0xFFULL << ((r) * 8))
#define FILE_MASK(f) (0x0101010101010101ULL << (f))

uint64_t generate_rook_mask(uint64_t blockers, uint8_t sq) {
    uint64_t attacks = 0ULL;
    int rank = sq / 8, file = sq % 8;

    // East
    for (int f = file + 1; f < 8; f++) {
        int s = rank * 8 + f;
        attacks |= (1ULL << s);
        if (blockers & (1ULL << s)) break;
    }
    // West
    for (int f = file - 1; f >= 0; f--) {
        int s = rank * 8 + f;
        attacks |= (1ULL << s);
        if (blockers & (1ULL << s)) break;
    }
    // North
    for (int r = rank + 1; r < 8; r++) {
        int s = r * 8 + file;
        attacks |= (1ULL << s);
        if (blockers & (1ULL << s)) break;
    }
    // South
    for (int r = rank - 1; r >= 0; r--) {
        int s = r * 8 + file;
        attacks |= (1ULL << s);
        if (blockers & (1ULL << s)) break;
    }
    return attacks;
}

int main() {
    FILE *f_attacks = fopen("bin/rook_attacks.bin", "wb");
    FILE *f_offsets = fopen("bin/rook_offsets.bin", "wb");
    if (!f_attacks || !f_offsets) {
        perror("Failed to open output file");
        return 1;
    }

    uint64_t total_offset = 0;

    for (int sq = 0; sq < 64; sq++) {
        uint64_t edge_mask = 0;
        int rank = sq / 8, file = sq % 8;

        // Remove edges (can't slide past board edges)
        if (file != 0) edge_mask |= MASK_FILE_A;
        if (file != 7) edge_mask |= MASK_FILE_H;
        if (rank != 0) edge_mask |= MASK_RANK_1;
        if (rank != 7) edge_mask |= MASK_RANK_8;

        uint64_t mask = (RANK_MASK(rank) | FILE_MASK(file)) & ~edge_mask & ~(1ULL << sq);

        // Collect relevant bits
        int relevant_bits = 0;
        uint8_t rel_index[12];
        for (int i = 0; i < 64; i++) {
            if (mask & (1ULL << i)) rel_index[relevant_bits++] = i;
        }

        uint16_t num_configs = 1 << relevant_bits;

        uint64_t *attacks = malloc(sizeof(uint64_t) * num_configs);
        if (!attacks) {
            perror("malloc failed");
            return 1;
        }

        for (uint16_t i = 0; i < num_configs; i++) {
            uint64_t blocker = 0;
            for (int j = 0; j < relevant_bits; j++) {
                if (i & (1 << j)) blocker |= (1ULL << rel_index[j]);
            }

            uint64_t attack = generate_rook_mask(blocker, sq);

            uint64_t magic = MAGIC_ROOK_NUMS[sq];
            uint8_t shift = MAGIC_ROOK_SHIFTS[sq];
            uint64_t index = (blocker * magic) >> (64 - shift);
            attacks[index] = attack;
        }

        // Write offset and length for this square
        uint32_t offset = (uint32_t)total_offset;
        uint32_t length = (1 << MAGIC_ROOK_SHIFTS[sq]);
        fwrite(&offset, sizeof(uint32_t), 1, f_offsets);
        fwrite(&length, sizeof(uint32_t), 1, f_offsets);

        // Write attack table entries
        fwrite(attacks, sizeof(uint64_t), length, f_attacks);
        total_offset += length;

        printf("Wrote rook attacks for square %d (%d entries)\n", sq, length);
        free(attacks);
    }

    fclose(f_attacks);
    fclose(f_offsets);
    return 0;
}
