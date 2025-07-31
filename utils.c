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