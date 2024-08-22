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

#include "bitboard.h"
#include "bitwise.h"
#include "types.h"

namespace Zagreus {
uint64_t Bitboard::getValue() const {
    return this->bitboard;
}

void Bitboard::setValue(uint64_t value) {
    this->bitboard = value;
}

uint64_t Bitboard::popcnt() {
    return Bitwise::popcnt(this->bitboard);
}

uint64_t Bitboard::popLsb() {
    return Bitwise::popLsb(this->bitboard);
}

int Bitboard::bitscanForward() {
    return Bitwise::bitscanForward(this->bitboard);
}

int Bitboard::bitscanReverse() {
    return Bitwise::bitscanReverse(this->bitboard);
}

template <Direction direction>
Bitboard Bitboard::shift() {
    Bitboard result{};

    switch (direction) {
        case NORTH:
            result.setValue(Bitwise::shiftNorth(this->bitboard));
            break;
        case SOUTH:
            result.setValue(Bitwise::shiftSouth(this->bitboard));
            break;
        case EAST:
            result.setValue(Bitwise::shiftEast(this->bitboard));
            break;
        case WEST:
            result.setValue(Bitwise::shiftWest(this->bitboard));
            break;
        case NORTH_EAST:
            result.setValue(Bitwise::shiftNorthEast(this->bitboard));
            break;
        case NORTH_WEST:
            result.setValue(Bitwise::shiftNorthWest(this->bitboard));
            break;
        case SOUTH_EAST:
            result.setValue(Bitwise::shiftSouthEast(this->bitboard));
            break;
        case SOUTH_WEST:
            result.setValue(Bitwise::shiftSouthWest(this->bitboard));
            break;
    }

    return result;
}

Bitboard Bitboard::operator&(const Bitboard& other) {
    Bitboard result{};

    result.setValue(this->getValue() & other.getValue());
    return result;
}

Bitboard Bitboard::operator|(const Bitboard& other) {
    Bitboard result{};

    result.setValue(this->getValue() | other.getValue());
    return result;
}

Bitboard Bitboard::operator^(const Bitboard& other) {
    Bitboard result{};

    result.setValue(this->getValue() ^ other.getValue());
    return result;
}

Bitboard Bitboard::operator~() {
    Bitboard result{};

    result.setValue(~this->getValue());
    return result;
}

Bitboard Bitboard::operator<<(int shift) {
    Bitboard result{};

    result.setValue(this->getValue() << shift);
    return result;
}

Bitboard Bitboard::operator>>(int shift) {
    Bitboard result{};

    result.setValue(this->getValue() >> shift);
    return result;
}

Bitboard& Bitboard::operator&=(const Bitboard& other) {
    this->setValue(this->getValue() & other.getValue());

    return *this;
}

Bitboard& Bitboard::operator|=(const Bitboard& other) {
    this->setValue(this->getValue() | other.getValue());

    return *this;
}

Bitboard& Bitboard::operator^=(const Bitboard& other) {
    this->setValue(this->getValue() ^ other.getValue());

    return *this;
}

Bitboard squareToBitboard(int index) {
    Bitboard result{};

    result.setValue(1ULL << index);
    return result;
}
} // namespace Zagreus