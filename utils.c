//
// Created by lenovo on 7/31/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "constants.h"
#include "utils.h"


uint64_t square_bit(int row, int col) {
    return 1ULL << ((7 - row) * 8 + col);
}

uint8_t rank_from_bit(uint64_t sq) {
    return sq & 7;
}
uint8_t file_from_bit(uint64_t sq) {
    return sq >> 3;
}