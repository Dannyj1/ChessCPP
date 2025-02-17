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
#include <cassert>
#include <cstdint>

#include "types.h"

namespace Zagreus {

/**
 * \brief Counts the number of set bits (population count) in a bitboard.
 * \param bb The bitboard to count the set bits in.
 * \return The number of set bits.
 */
inline uint64_t popcnt(uint64_t bb) {
#ifdef _MSC_VER
    return __popcnt64(bb);
#else
    return __builtin_popcountll(bb);
#endif
}

/**
 * \brief Finds the index of the least significant set bit (LSB) in a bitboard.
 * \param bb The bitboard to scan.
 * \return The index of the least significant set bit.
 */
inline int bitscanForward(uint64_t bb) {
    assert(bb != 0);
#ifdef _MSC_VER
    unsigned long index;
    _BitScanForward64(&index, bb);
    return (int) index;
#else
    return __builtin_ctzll(bb);
#endif
}

/**
 * \brief Finds the index of the most significant set bit (MSB) in a bitboard.
 * \param bb The bitboard to scan.
 * \return The index of the most significant set bit.
 */
inline int bitscanReverse(const uint64_t bb) {
    assert(bb != 0);
#ifdef _MSC_VER
    unsigned long index;
    _BitScanReverse64(&index, bb);
    return (int) index;
#else
    return 63 - __builtin_clzll(bb);
#endif
}

/**
 * \brief Pops the least significant set bit (LSB) from a bitboard and returns its index. This modifies the bitboard.
 * \param bb The bitboard to modify.
 * \return The index of the popped least significant set bit.
 */
inline int popLsb(uint64_t& bb) {
    assert(bb != 0);
    const int lsb = bitscanForward(bb);
    assert(lsb >= 0 && lsb < 64);

    bb &= bb - 1;
    return lsb;
}

} // namespace Zagreus::Bitwise