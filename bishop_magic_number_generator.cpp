#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// File and rank masks
#define MASK_FILE_A 0x0101010101010101ULL
#define MASK_FILE_H 0x8080808080808080ULL
#define MASK_RANK_1 0x00000000000000FFULL
#define MASK_RANK_8 0xFF00000000000000ULL
#define EDGE_MASK   (MASK_FILE_A | MASK_FILE_H | MASK_RANK_1 | MASK_RANK_8)

// Generate a random 64-bit number
uint64_t random_u64() {
    return ((uint64_t)rand() & 0xFFFF) << 48 |
           ((uint64_t)rand() & 0xFFFF) << 32 |
           ((uint64_t)rand() & 0xFFFF) << 16 |
           ((uint64_t)rand() & 0xFFFF);
}

// Compute bishop movement mask excluding edges
uint64_t bishop_attack_mask(int sq) {
    uint64_t mask = 0ULL;
    int r = sq / 8;
    int f = sq % 8;

    for (int i = 1; r + i < 7 && f + i < 7; i++) mask |= 1ULL << ((r + i) * 8 + (f + i));
    for (int i = 1; r + i < 7 && f - i > 0;  i++) mask |= 1ULL << ((r + i) * 8 + (f - i));
    for (int i = 1; r - i > 0 && f + i < 7; i++) mask |= 1ULL << ((r - i) * 8 + (f + i));
    for (int i = 1; r - i > 0 && f - i > 0;  i++) mask |= 1ULL << ((r - i) * 8 + (f - i));

    return mask;
}

int count_bits(uint64_t bb) {
    int count = 0;
    while (bb) {
        count += bb & 1;
        bb >>= 1;
    }
    return count;
}

int main() {
    srand(time(NULL));
    for (int sq = 0; sq < 64; sq++) {
        uint64_t attack_mask = bishop_attack_mask(sq);
        int relevant_bits = count_bits(attack_mask);
        int num_configs = 1 << relevant_bits;

        uint8_t indices[14];
        int idx = 0;
        for (int i = 0; i < 64; i++) {
            if (attack_mask & (1ULL << i)) {
                indices[idx++] = i;
            }
        }

        uint64_t blockers[num_configs];
        for (int i = 0; i < num_configs; i++) {
            uint64_t b = 0ULL;
            for (int j = 0; j < relevant_bits; j++) {
                if (i & (1 << j)) b |= (1ULL << indices[j]);
            }
            blockers[i] = b;
        }

        // Try magic numbers
        uint64_t magic = 0;
        for (int attempts = 0; attempts < 1000000; attempts++) {
            uint64_t candidate = random_u64() & random_u64() & random_u64();
            int table_size = 1 << relevant_bits;
            bool used[table_size];
            for (int i = 0; i < table_size; i++) used[i] = false;

            bool collision = false;
            for (int i = 0; i < num_configs; i++) {
                uint64_t index = (blockers[i] * candidate) >> (64 - relevant_bits);
                if (used[index]) {
                    collision = true;
                    break;
                }
                used[index] = true;
            }

            if (!collision) {
                magic = candidate;
                break;
            }
        }

        if (magic) {
            printf("#define MAGIC_BISHOP_NUM_%d 0x%llxULL\n", sq, magic);
            printf("#define MAGIC_BISHOP_SHIFT_%d %d\n\n", sq, relevant_bits);
        } else {
            printf("// Failed to find magic number for square %d\n", sq);
        }
    }

    return 0;
}
