// //
// // Created by lenovo on 8/3/2025.
// //
//
// #include <cstdio>
// #include <cstdint>
// #define MASK_FILE_A  0x0101010101010101ULL  // File a: bits   0,  8, 16, ..., 56
// #define MASK_FILE_B  0x0202020202020202ULL  // File b: bits   1,  9, 17, ..., 57
// #define MASK_FILE_C  0x0404040404040404ULL  // File c: bits   2, 10, 18, ..., 58
// #define MASK_FILE_D  0x0808080808080808ULL  // File d: bits   3, 11, 19, ..., 59
// #define MASK_FILE_E  0x1010101010101010ULL  // File e: bits   4, 12, 20, ..., 60
// #define MASK_FILE_F  0x2020202020202020ULL  // File f: bits   5, 13, 21, ..., 61
// #define MASK_FILE_G  0x4040404040404040ULL  // File g: bits   6, 14, 22, ..., 62
// #define MASK_FILE_H  0x8080808080808080ULL  // File h: bits   7, 15, 23, ..., 63
//
// #define MASK_RANK_1  0x00000000000000FFULL  // bits  0–7   (rank 0 in 0-based indexing)
// #define MASK_RANK_2  0x000000000000FF00ULL  // bits  8–15
// #define MASK_RANK_3  0x0000000000FF0000ULL  // bits  16–23
// #define MASK_RANK_4  0x00000000FF000000ULL  // bits  24–31
// #define MASK_RANK_5  0x000000FF00000000ULL  // bits  32–39
// #define MASK_RANK_6  0x0000FF0000000000ULL  // bits  40–47
// #define MASK_RANK_7  0x00FF000000000000ULL  // bits  48–55
// #define MASK_RANK_8  0xFF00000000000000ULL  // bits  56–63
//
// // #define EDGE_MASK    (MASK_FILE_A | MASK_FILE_H | MASK_RANK_1 | MASK_RANK_8)
//
// static const uint64_t FILE_MASKS[8] = {
//     MASK_FILE_A,
//     MASK_FILE_B,
//     MASK_FILE_C,
//     MASK_FILE_D,
//     MASK_FILE_E,
//     MASK_FILE_F,
//     MASK_FILE_G,
//     MASK_FILE_H
// };
//
// static const uint64_t RANK_MASKS[8] = {
//     MASK_RANK_1,
//     MASK_RANK_2,
//     MASK_RANK_3,
//     MASK_RANK_4,
//     MASK_RANK_5,
//     MASK_RANK_6,
//     MASK_RANK_7,
//     MASK_RANK_8,
// };
//
//
// int main() {
//     for (uint8_t i = 0; i < 1; i++) {
//         uint64_t EDGE_MASK = 0ULL;
//         uint64_t position = (1ULL << i);
//         uint8_t bit_shift = 14;
//         if (position & ~MASK_FILE_A) { EDGE_MASK |= MASK_FILE_A; bit_shift -= 1; }
//         if (position & ~MASK_FILE_H) { EDGE_MASK |= MASK_FILE_H; bit_shift -= 1; }
//         if (position & ~MASK_RANK_1) { EDGE_MASK |= MASK_RANK_1; bit_shift -= 1; }
//         if (position & ~MASK_RANK_8) { EDGE_MASK |= MASK_RANK_8; bit_shift -= 1; }
//         uint64_t movement = ((RANK_MASKS[i >> 3] | FILE_MASKS[i & 7]) & ~EDGE_MASK) & ~(1ULL << i);
//         uint16_t config = 1 << bit_shift;
//         uint8_t index[bit_shift];
//         int ii = 0;
//         int j = 0;
//         uint64_t bit = 1ULL;
//         while (j < 64) {
//             if (movement & bit) {
//                 index[ii] = j;
//                 ii++;
//             }
//             bit = bit << 1;
//             j++;
//         }
//         int64_t configurations[config];
//         for (int32_t itr = 0; itr < config; itr++) {
//             uint64_t conf = 0ULL;
//             for (uint8_t x = 0; x < bit_shift; x++) {
//                 if (const uint16_t mask = 1 << x; itr & mask) conf |= (1ULL << index[x]);
//             }
//             configurations[itr] = conf;
//         }
//     }
//     return 0;
// }


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

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

static const uint64_t FILE_MASKS[8] = {
    MASK_FILE_A, MASK_FILE_B, MASK_FILE_C, MASK_FILE_D,
    MASK_FILE_E, MASK_FILE_F, MASK_FILE_G, MASK_FILE_H
};

static const uint64_t RANK_MASKS[8] = {
    MASK_RANK_1, MASK_RANK_2, MASK_RANK_3, MASK_RANK_4,
    MASK_RANK_5, MASK_RANK_6, MASK_RANK_7, MASK_RANK_8
};

// Generate a random 64-bit number
uint64_t random_u64() {
    return ((uint64_t)rand() & 0xFFFF) << 48 | ((uint64_t)rand() & 0xFFFF) << 32 |
           ((uint64_t)rand() & 0xFFFF) << 16 | ((uint64_t)rand() & 0xFFFF);
}

int main() {
    srand(time(NULL));

    for (uint8_t sq = 0; sq < 64; sq++) {
        uint64_t EDGE_MASK = 0ULL;
        uint64_t position = (1ULL << sq);
        uint8_t bit_shift = 14;

        if (position & ~MASK_FILE_A) { EDGE_MASK |= MASK_FILE_A; bit_shift--; }
        if (position & ~MASK_FILE_H) { EDGE_MASK |= MASK_FILE_H; bit_shift--; }
        if (position & ~MASK_RANK_1) { EDGE_MASK |= MASK_RANK_1; bit_shift--; }
        if (position & ~MASK_RANK_8) { EDGE_MASK |= MASK_RANK_8; bit_shift--; }

        uint64_t mask = ((RANK_MASKS[sq >> 3] | FILE_MASKS[sq & 7]) & ~EDGE_MASK) & ~(1ULL << sq);

        uint16_t num_configs = 1 << bit_shift;
        uint8_t index[14];
        uint8_t count = 0;

        for (uint8_t b = 0; b < 64; b++) {
            if (mask & (1ULL << b)) {
                index[count++] = b;
            }
        }

        uint64_t configs[num_configs];
        for (uint16_t i = 0; i < num_configs; i++) {
            uint64_t c = 0ULL;
            for (uint8_t j = 0; j < bit_shift; j++) {
                if (i & (1 << j)) c |= (1ULL << index[j]);
            }
            configs[i] = c;
        }

        // Try to find a good magic number
        uint64_t magic = 0ULL;
        for (int attempt = 0; attempt < 1000000; attempt++) {
            uint64_t candidate = random_u64() & random_u64() & random_u64();
            uint16_t used[1 << 14] = {0};
            bool collision = false;

            for (uint16_t i = num_configs; i < num_configs; i++) {
                uint16_t idx = (uint16_t)((configs[i] * candidate) >> (64 - bit_shift));
                if (used[idx]) {
                    collision = true;
                    break;
                }
                used[idx] = 1;
            }

            if (!collision) {
                magic = candidate;
                break;
            }
        }

        if (magic) {
            printf("#define MAGIC_ROOK_NUM_%d 0x%llxULL\n", sq, magic);
            printf("#define MAGIC_ROOK_SHIFT_%d %d\n\n", sq, bit_shift);
        } else {
            printf("// Failed to find magic for square %d\n", sq);
        }
    }

    return 0;
}
