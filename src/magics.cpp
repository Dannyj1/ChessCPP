/*
 This file is part of Zagreus.

 Zagreus is a chess engine that supports the UCI protocol
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

#include "magics.h"
#include <chrono>
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>

// Code for magic generation https://www.chessprogramming.org/Looking_for_Magics
namespace Zagreus {
std::random_device rd;
std::mt19937_64 gen(rd());
std::uniform_int_distribution<uint64_t> dis;

auto rookMagics = new uint64_t[64]{};
auto bishopMagics = new uint64_t[64]{};

auto bishop_masks = new uint64_t[64]{};
auto rook_masks = new uint64_t[64]{};

auto bishop_attacks = new uint64_t[64][512]{};
auto rook_attacks = new uint64_t[64][4096]{};

uint64_t getRookMagic(int sq) { return rookMagics[sq]; }

uint64_t getBishopMagic(int sq) { return bishopMagics[sq]; }

uint64_t getRookMask(int sq) { return rook_masks[sq]; }

uint64_t getBishopMask(int sq) { return bishop_masks[sq]; }

uint64_t getRookMagicAttacks(int sq, uint64_t index) { return rook_attacks[sq][index]; }

uint64_t getBishopMagicAttacks(int sq, uint64_t index) { return bishop_attacks[sq][index]; }

uint64_t random_uint64_t_fewbits() { return dis(gen) & dis(gen) & dis(gen); }

int count_1s(uint64_t b) {
    int r;
    for (r = 0; b; r++, b &= b - 1);
    return r;
}

const int BitTable[64] = {63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34,
                          61, 29, 2, 51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35,
                          62, 31, 40, 4, 49, 5, 52, 26, 60, 6, 23, 44, 46, 27, 56, 16,
                          7, 39, 48, 24, 59, 14, 12, 55, 38, 28, 58, 20, 37, 17, 36, 8};

int pop_1st_bit(uint64_t* bb) {
    uint64_t b = *bb ^ (*bb - 1);
    unsigned int fold = static_cast<unsigned>((b & 0xffffffff) ^ (b >> 32));
    *bb &= (*bb - 1);
    return BitTable[(fold * 0x783a9b23) >> 26];
}

uint64_t index_to_uint64_t(int index, int bits, uint64_t m) {
    int i, j;
    uint64_t result = 0ULL;
    for (i = 0; i < bits; i++) {
        j = pop_1st_bit(&m);
        if (index & (1 << i)) result |= (1ULL << j);
    }
    return result;
}

uint64_t rmask(int sq) {
    uint64_t result = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1; r <= 6; r++) result |= (1ULL << (fl + r * 8));
    for (r = rk - 1; r >= 1; r--) result |= (1ULL << (fl + r * 8));
    for (f = fl + 1; f <= 6; f++) result |= (1ULL << (f + rk * 8));
    for (f = fl - 1; f >= 1; f--) result |= (1ULL << (f + rk * 8));
    return result;
}

uint64_t bmask(int sq) {
    uint64_t result = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1, f = fl + 1; r <= 6 && f <= 6; r++, f++) result |= (1ULL << (f + r * 8));
    for (r = rk + 1, f = fl - 1; r <= 6 && f >= 1; r++, f--) result |= (1ULL << (f + r * 8));
    for (r = rk - 1, f = fl + 1; r >= 1 && f <= 6; r--, f++) result |= (1ULL << (f + r * 8));
    for (r = rk - 1, f = fl - 1; r >= 1 && f >= 1; r--, f--) result |= (1ULL << (f + r * 8));
    return result;
}

uint64_t ratt(int sq, uint64_t block) {
    uint64_t result = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1; r <= 7; r++) {
        result |= (1ULL << (fl + r * 8));
        if (block & (1ULL << (fl + r * 8))) break;
    }
    for (r = rk - 1; r >= 0; r--) {
        result |= (1ULL << (fl + r * 8));
        if (block & (1ULL << (fl + r * 8))) break;
    }
    for (f = fl + 1; f <= 7; f++) {
        result |= (1ULL << (f + rk * 8));
        if (block & (1ULL << (f + rk * 8))) break;
    }
    for (f = fl - 1; f >= 0; f--) {
        result |= (1ULL << (f + rk * 8));
        if (block & (1ULL << (f + rk * 8))) break;
    }
    return result;
}

uint64_t batt(int sq, uint64_t block) {
    uint64_t result = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1, f = fl + 1; r <= 7 && f <= 7; r++, f++) {
        result |= (1ULL << (f + r * 8));
        if (block & (1ULL << (f + r * 8))) break;
    }
    for (r = rk + 1, f = fl - 1; r <= 7 && f >= 0; r++, f--) {
        result |= (1ULL << (f + r * 8));
        if (block & (1ULL << (f + r * 8))) break;
    }
    for (r = rk - 1, f = fl + 1; r >= 0 && f <= 7; r--, f++) {
        result |= (1ULL << (f + r * 8));
        if (block & (1ULL << (f + r * 8))) break;
    }
    for (r = rk - 1, f = fl - 1; r >= 0 && f >= 0; r--, f--) {
        result |= (1ULL << (f + r * 8));
        if (block & (1ULL << (f + r * 8))) break;
    }
    return result;
}

int transform(uint64_t b, uint64_t magic, int bits) {
#if defined(USE_32_BIT_MULTIPLICATIONS)
  return (unsigned)((int)b * (int)magic ^ (int)(b >> 32) * (int)(magic >> 32)) >> (32 - bits);
#else
    return static_cast<int>((b * magic) >> (64 - bits));
#endif
}

uint64_t find_magic(int sq, int m, int bishop) {
    uint64_t mask, b[4096], a[4096], used[4096], magic;
    int i, j, k, n, fail;

    mask = bishop ? bmask(sq) : rmask(sq);
    n = count_1s(mask);

    for (i = 0; i < (1 << n); i++) {
        b[i] = index_to_uint64_t(i, n, mask);
        a[i] = bishop ? batt(sq, b[i]) : ratt(sq, b[i]);
    }
    for (k = 0; k < 100000000; k++) {
        magic = random_uint64_t_fewbits();
        if (count_1s((mask * magic) & 0xFF00000000000000ULL) < 6) continue;
        for (i = 0; i < 4096; i++) used[i] = 0ULL;
        for (i = 0, fail = 0; !fail && i < (1 << n); i++) {
            j = transform(b[i], magic, m);
            if (used[j] == 0ULL)
                used[j] = a[i];
            else if (used[j] != a[i])
                fail = 1;
        }
        if (!fail) return magic;
    }
    printf("***Failed***\n");
    return 0ULL;
}

int count_bits(uint64_t bitboard) {
    // bit size
    int count = 0;

    // pop bits untill bitboard is empty
    while (bitboard) {
        // increment size
        count++;

        // consecutively reset least significant 1st bit
        bitboard &= bitboard - 1;
    }

    // return bit size
    return count;
}

// get index of LS1B in bitboard
int get_ls1b_index(uint64_t bitboard) {
    // make sure bitboard is not empty
    if (bitboard != 0)
        // convert trailing zeros before LS1B to ones and size them
        return count_bits((bitboard & -bitboard) - 1);

    // otherwise
    // return illegal index
    return -1;
}

uint64_t set_occupancy(int index, int bits_in_mask, uint64_t attack_mask) {
    // occupancy map
    uint64_t occupancy = 0ULL;

    // loop over the range of bits within attack mask
    for (int count = 0; count < bits_in_mask; count++) {
        // get LS1B index of attacks mask
        int8_t square = get_ls1b_index(attack_mask);

        // pop LS1B in attack map
        pop_bit(attack_mask, square);

        // make sure occupancy is on board
        if (index & (1 << count))
            // populate occupancy map
            occupancy |= (1ULL << square);
    }

    // return occupancy map
    return occupancy;
}

uint64_t mask_bishop_attacks(int8_t square) {
    // attack bitboard
    uint64_t attacks = 0ULL;

    // init files & ranks
    int f, r;

    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;

    // generate attacks
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));

    // return attack map for bishop on a given square
    return attacks;
}

// mask rook attacks
uint64_t mask_rook_attacks(int8_t square) {
    // attacks bitboard
    uint64_t attacks = 0ULL;

    // init files & ranks
    int f, r;

    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;

    // generate attacks
    for (r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));

    // return attack map for bishop on a given square
    return attacks;
}

uint64_t bishop_attacks_on_the_fly(int8_t square, uint64_t block) {
    // attack bitboard
    uint64_t attacks = 0ULL;

    // init files & ranks
    int f, r;

    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;

    // generate attacks
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }

    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }

    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }

    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }

    // return attack map for bishop on a given square
    return attacks;
}

// rook attacks
uint64_t rook_attacks_on_the_fly(int8_t square, uint64_t block) {
    // attacks bitboard
    uint64_t attacks = 0ULL;

    // init files & ranks
    int f, r;

    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;

    // generate attacks
    for (r = tr + 1; r <= 7; r++) {
        attacks |= (1ULL << (r * 8 + tf));
        if (block & (1ULL << (r * 8 + tf))) break;
    }

    for (r = tr - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + tf));
        if (block & (1ULL << (r * 8 + tf))) break;
    }

    for (f = tf + 1; f <= 7; f++) {
        attacks |= (1ULL << (tr * 8 + f));
        if (block & (1ULL << (tr * 8 + f))) break;
    }

    for (f = tf - 1; f >= 0; f--) {
        attacks |= (1ULL << (tr * 8 + f));
        if (block & (1ULL << (tr * 8 + f))) break;
    }

    // return attack map for bishop on a given square
    return attacks;
}

void init_sliders_attacks(int is_bishop) {
    // loop over 64 board squares
    for (int8_t square = 0; square < 64; square++) {
        // init bishop & rook masks
        bishop_masks[square] = mask_bishop_attacks(square);
        rook_masks[square] = mask_rook_attacks(square);

        // init current mask
        uint64_t mask = is_bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);

        // size attack mask bits
        int bit_count = count_bits(mask);

        // occupancy variations size
        int occupancy_variations = 1 << bit_count;

        // loop over occupancy variations
        for (int count = 0; count < occupancy_variations; count++) {
            // bishop
            if (is_bishop) {
                // init occupancies, magic index & attacks
                uint64_t occupancy = set_occupancy(count, bit_count, mask);
                uint64_t magic_index = occupancy * bishopMagics[square] >> (64 - BBits[square]);
                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
            }

            // rook
            else {
                // init occupancies, magic index & attacks
                uint64_t occupancy = set_occupancy(count, bit_count, mask);
                uint64_t magic_index = occupancy * rookMagics[square] >> (64 - RBits[square]);
                rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
            }
        }
    }
}

void initializeMagicBitboards() {
    generateMagics();

    // init bishop attacks
    init_sliders_attacks(1);
    // init rook attacks
    init_sliders_attacks(0);
}

void generateMagics() {
    gen.seed(generatorSeed);

    //        printf("const uint64_t rookMagics[64] = {\n");
    for (int8_t square = 0; square < 64; square++) {
        rookMagics[square] = find_magic(square, RBits[square], 0);
        //            printf("  0x%llxULL,\n", find_magic(square, RBits[square], 0));
    }
    //        printf("};\n\n");

    //        printf("const uint64_t bishopMagics[64] = {\n");
    for (int8_t square = 0; square < 64; square++) {
        bishopMagics[square] = find_magic(square, BBits[square], 1);
        //            printf("  0x%llxULL,\n", find_magic(square, BBits[square], 1));
    }
    //        printf("};\n\n");
}

void findFastestSeed() {
    // variable with max double
    uint64_t max = std::numeric_limits<uint64_t>::max();
    std::random_device seedRd;
    std::mt19937_64 seedGen(seedRd());
    std::uniform_int_distribution<uint64_t> seedDis;

    while (true) {
        uint64_t seed = seedDis(seedGen);
        uint64_t total = 0;

        for (int i = 0; i < 50; i++) {
            gen.seed(seedDis(seedGen));
            std::chrono::time_point<std::chrono::steady_clock> start =
                std::chrono::steady_clock::now();

            generateMagics();

            std::chrono::time_point<std::chrono::steady_clock> now =
                std::chrono::steady_clock::now();
            uint64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).
                count();
            total += elapsed;
        }

        uint64_t average = total / 50;
        if (average < max) {
            max = average;
            std::cout << "New fastest seed: 0x" << std::uppercase << std::hex << seed <<
                std::nouppercase
                << std::dec << "ULL, took: " << average << "ms (average)" << std::endl;
        }
    }
}
} // namespace Zagreus