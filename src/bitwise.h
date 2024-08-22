
/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2024  Danny Jelsma

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

namespace Zagreus::Bitwise {
inline uint64_t popcnt(uint64_t bb);

inline int bitscanForward(uint64_t bb);

inline int bitscanReverse(uint64_t bb);

inline uint64_t popLsb(uint64_t& bb);

inline uint64_t shiftNorth(uint64_t bb);

inline uint64_t shiftSouth(uint64_t bb);

inline uint64_t shiftEast(uint64_t bb);

inline uint64_t shiftWest(uint64_t bb);

inline uint64_t shiftNorthEast(uint64_t bb);

inline uint64_t shiftNorthWest(uint64_t bb);

inline uint64_t shiftSouthEast(uint64_t bb);

inline uint64_t shiftSouthWest(uint64_t bb);
} // namespace Zagreus::Bitwise
