//
// Created by lenovo on 7/31/2025.
//

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

uint64_t square_bit(int row, int col);
uint8_t rank_from_bit(uint64_t sq);
uint8_t file_from_bit(uint64_t sq);

#endif //UTILS_H
