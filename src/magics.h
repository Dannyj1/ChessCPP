/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2025  Danny Jelsma

 Zagreus is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Zagreus is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with Zagreus.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <cstdint>

namespace Zagreus {
constexpr uint64_t generatorSeed = 0x1C6FE234A7121C08ULL;

#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? (bitboard ^= (1ULL << square)) : 0)

static int RBits[64] = {12, 11, 11, 11, 11, 11, 11, 12, 11, 10, 10, 10, 10, 10, 10, 11,
                        11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11,
                        11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11,
                        11, 10, 10, 10, 10, 10, 10, 11, 12, 11, 11, 11, 11, 11, 11, 12};

static int BBits[64] = {6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7,
                        5, 5, 5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 7,
                        7, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6};

uint64_t getRookMagic(int sq);

uint64_t getBishopMagic(int sq);

uint64_t getRookMask(int sq);

uint64_t getBishopMask(int sq);

uint64_t getRookMagicAttacks(int sq, uint64_t block);

uint64_t getBishopMagicAttacks(int sq, uint64_t block);

uint64_t random_uint64_t_fewbits();

int count_1s(uint64_t b);

int pop_1st_bit(uint64_t* bb);

uint64_t index_to_uint64_t(int index, int bits, uint64_t m);

uint64_t rmask(int sq);

uint64_t bmask(int sq);

uint64_t ratt(int sq, uint64_t block);

uint64_t batt(int sq, uint64_t block);

int transform(uint64_t b, uint64_t magic, int bits);

uint64_t find_magic(int sq, int m, int bishop);

void generateMagics();

void findFastestSeed();

void init_sliders_attacks(int is_bishop);

uint64_t set_occupancy(int index, int bits_in_mask, uint64_t attack_mask);

int get_ls1b_index(uint64_t bitboard);

uint64_t mask_rook_attacks(int8_t square);

uint64_t mask_bishop_attacks(int8_t square);

int count_bits(uint64_t bitboard);

uint64_t rook_attacks_on_the_fly(int8_t square, uint64_t block);

uint64_t bishop_attacks_on_the_fly(int8_t square, uint64_t block);

void initializeMagicBitboards();
} // namespace Zagreus