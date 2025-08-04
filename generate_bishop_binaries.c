//
// Created by lenovo on 8/4/2025.
//
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "magic.h"

// File and rank masks
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
#define EDGE_MASK   (MASK_FILE_A | MASK_FILE_H | MASK_RANK_1 | MASK_RANK_8)

#define REL_9       (MASK_FILE_D | MASK_FILE_E)&(MASK_RANK_4 | MASK_RANK_5)
#define REL_7       (MASK_FILE_F | MASK_FILE_E | MASK_FILE_D | MASK_FILE_C)&(MASK_RANK_6 | MASK_RANK_5 | MASK_RANK_4 | MASK_RANK_3) & ~REL_9
#define REL_6       (MASK_FILE_A | MASK_FILE_H)&(MASK_RANK_1 | MASK_RANK_8)
#define REL_5       ~REL_7 & ~REL_6

// uint64_t bishop_attack_mask(int sq) {
//     uint64_t mask = 0ULL;
//     int r = sq / 8;
//     int f = sq % 8;
//
//     for (int i = 1; r + i < 7 && f + i < 7; i++) mask |= 1ULL << ((r + i) * 8 + (f + i));
//     for (int i = 1; r + i < 7 && f - i > 0;  i++) mask |= 1ULL << ((r + i) * 8 + (f - i));
//     for (int i = 1; r - i > 0 && f + i < 7; i++) mask |= 1ULL << ((r - i) * 8 + (f + i));
//     for (int i = 1; r - i > 0 && f - i > 0;  i++) mask |= 1ULL << ((r - i) * 8 + (f - i));
//
//     return mask;
// }
//
// uint8_t get_relevant_bits(const int8_t sq) {
//     const uint64_t position = 1ULL << sq;
//     if (position & REL_9) return 9;
//     if (position & REL_6) return 6;
//     if (position & REL_7) return 7;
//     return 5;
// }
//
// // int count_bits(uint64_t bb) {
// //     int count = 0;
// //     while (bb) {
// //         count += bb & 1;
// //         bb >>= 1;
// //     }
// //     return count;
// // }
//
// uint64_t generate_bishop_mask(uint64_t blockers, uint8_t sq) {
//     uint8_t rank = sq >> 3;
//     uint8_t file = sq & 7;
//     uint64_t bishop_mask = 0ULL;
//
//     // NE (up-right)
//     for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++) {
//         int i = r * 8 + f;
//         uint64_t mask = 1ULL << i;
//         bishop_mask |= mask;
//         if (mask & blockers) break;
//     }
//
//     // NW (up-left)
//     for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--) {
//         int i = r * 8 + f;
//         uint64_t mask = 1ULL << i;
//         bishop_mask |= mask;
//         if (mask & blockers) break;
//     }
//
//     // SE (down-right)
//     for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++) {
//         int i = r * 8 + f;
//         uint64_t mask = 1ULL << i;
//         bishop_mask |= mask;
//         if (mask & blockers) break;
//     }
//
//     // SW (down-left)
//     for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
//         int i = r * 8 + f;
//         uint64_t mask = 1ULL << i;
//         bishop_mask |= mask;
//         if (mask & blockers) break;
//     }
//
//     return bishop_mask;
// }
// // void print_board(uint64_t blockers, uint64_t attacks) {
// //     for (int rank = 7; rank >= 0; rank--) {
// //         printf("%d  ", rank + 1);
// //         for (int file = 0; file < 8; file++) {
// //             int sq = rank * 8 + file;
// //             uint64_t mask = 1ULL << sq;
// //             if (blockers & mask & attacks)
// //                 printf("B ");
// //             else if (blockers & mask)
// //                 printf("b ");
// //             else if (attacks & mask)
// //                 printf("* ");
// //             else
// //                 printf(". ");
// //         }
// //         printf("\n");
// //     }
// //     printf("\n   a b c d e f g h\n\n");
// // }
//
// int main() {
//     for (int sq = 0; sq < 64; sq++) {
//         uint64_t attack_mask = bishop_attack_mask(sq);
//         int relevant_bits = get_relevant_bits(sq);
//         int num_configs = 1 << relevant_bits;
//
//         uint64_t lookup_table[num_configs];// worst case: max 2^14 entries
//         memset(lookup_table, 0, sizeof(lookup_table));
//
//         uint8_t indices[14];
//         int idx = 0;
//         for (int i = 0; i < 64; i++) {
//             if (attack_mask & (1ULL << i)) {
//                 indices[idx++] = i;
//             }
//         }
//
//         uint64_t blockers[num_configs];
//         for (int i = 0; i < num_configs; i++) {
//             uint64_t b = 0ULL;
//             for (int j = 0; j < relevant_bits; j++) {
//                 if (i & (1 << j)) b |= (1ULL << indices[j]);
//             }
//             blockers[i] = b;
//         }
//         uint64_t attacks[num_configs];
//         for (uint16_t i = 0; i < num_configs; i++) {
//             attacks[i] = generate_bishop_mask(blockers[i], sq);
//         }
//         // for (int i = 0; i < num_configs; i++) {
//         //     print_board(blockers[i], attacks[i]);
//         // }
//         for (int i = 0; i < num_configs; i++) {
//             uint64_t blocker = blockers[i];
//             uint64_t attack = generate_bishop_mask(blocker, sq);  // or rename to generate_bishop_attacks()
//
//             uint64_t magic = MAGIC_BISHOP_NUMS[sq];
//             uint8_t shift = MAGIC_BISHOP_SHIFTS[sq];
//
//             uint64_t idx = (blocker * magic) >> (64 - shift);
//             lookup_table[idx] = attack;
//
//             // // Optional: print for debugging
//             // printf("Blocker Config %u (index = %llu):\n", i, idx);
//             // print_board(blockers, attack);
//         }
//         char filename[64];
//         sprintf(filename, "bin/bishop_attacks_sq_%d.bin", sq);
//         FILE* f = fopen(filename, "wb");
//         fwrite(lookup_table, sizeof(uint64_t), 1 << MAGIC_BISHOP_SHIFTS[sq], f);
//         fclose(f);
//         printf("(%d/64)\n", sq+1);
//     }
//     return 0;
// }

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "magic.h"

// Mask Definitions (same as before)
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

#define REL_9  ((MASK_FILE_D | MASK_FILE_E) & (MASK_RANK_4 | MASK_RANK_5))
#define REL_7  (((MASK_FILE_C | MASK_FILE_D | MASK_FILE_E | MASK_FILE_F) & (MASK_RANK_3 | MASK_RANK_4 | MASK_RANK_5 | MASK_RANK_6)) & ~REL_9)
#define REL_6  ((MASK_FILE_A | MASK_FILE_H) & (MASK_RANK_1 | MASK_RANK_8))
#define REL_5  (~(REL_9 | REL_7 | REL_6))

// Bishop attack mask for blocker permutations (inner squares only)
uint64_t bishop_attack_mask(int sq) {
    uint64_t mask = 0ULL;
    int r = sq / 8, f = sq % 8;

    for (int i = 1; r + i < 7 && f + i < 7; i++) mask |= 1ULL << ((r + i) * 8 + (f + i));
    for (int i = 1; r + i < 7 && f - i > 0;  i++) mask |= 1ULL << ((r + i) * 8 + (f - i));
    for (int i = 1; r - i > 0 && f + i < 7; i++) mask |= 1ULL << ((r - i) * 8 + (f + i));
    for (int i = 1; r - i > 0 && f - i > 0;  i++) mask |= 1ULL << ((r - i) * 8 + (f - i));

    return mask;
}

// Map square to pre-defined relevant bits
uint8_t get_relevant_bits(int sq) {
    uint64_t pos = 1ULL << sq;
    if (pos & REL_9) return 9;
    if (pos & REL_6) return 6;
    if (pos & REL_7) return 7;
    return 5;
}

// Generate full bishop attack bitboard for a given blocker mask
uint64_t generate_bishop_mask(uint64_t blockers, uint8_t sq) {
    uint8_t rank = sq >> 3, file = sq & 7;
    uint64_t attacks = 0ULL;

    for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++) {
        uint64_t bit = 1ULL << (r * 8 + f);
        attacks |= bit; if (blockers & bit) break;
    }
    for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--) {
        uint64_t bit = 1ULL << (r * 8 + f);
        attacks |= bit; if (blockers & bit) break;
    }
    for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++) {
        uint64_t bit = 1ULL << (r * 8 + f);
        attacks |= bit; if (blockers & bit) break;
    }
    for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
        uint64_t bit = 1ULL << (r * 8 + f);
        attacks |= bit; if (blockers & bit) break;
    }

    return attacks;
}

int main() {
    uint64_t* all_attacks = malloc(1ULL << 20);  // max ~1MB
    uint32_t offsets[64] = {0};
    size_t total = 0;

    for (int sq = 0; sq < 64; sq++) {
        uint8_t rel_bits = get_relevant_bits(sq);
        int num_configs = 1 << rel_bits;

        uint8_t indices[14], count = 0;
        uint64_t mask = bishop_attack_mask(sq);
        for (int i = 0; i < 64; i++)
            if (mask & (1ULL << i)) indices[count++] = i;

        offsets[sq] = total;

        for (int i = 0; i < num_configs; i++) {
            uint64_t blockers = 0ULL;
            for (int j = 0; j < rel_bits; j++)
                if (i & (1 << j)) blockers |= (1ULL << indices[j]);

            uint64_t attack = generate_bishop_mask(blockers, sq);
            all_attacks[total++] = attack;
        }
    }

    // Write packed attack table
    FILE* f_attacks = fopen("bin/bishop_attacks.bin", "wb");
    fwrite(all_attacks, sizeof(uint64_t), total, f_attacks);
    fclose(f_attacks);

    // Write per-square offsets
    FILE* f_offsets = fopen("bin/bishop_offsets.bin", "wb");
    fwrite(offsets, sizeof(uint32_t), 64, f_offsets);
    fclose(f_offsets);

    printf("Generated bishop attack table with %zu entries (~%zu KB)\n", total, total * sizeof(uint64_t) / 1024);

    free(all_attacks);
    return 0;
}
