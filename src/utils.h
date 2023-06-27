/*
 This file is part of Zagreus.

 Zagreus is a chess engine that supports the UCI protocol
 Copyright (C) 2023  Danny Jelsma

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

#include <string>

#include "types.h"

namespace Zagreus {
    inline static PieceColor getOppositeColor(PieceColor color) {
        return static_cast<PieceColor>(color ^ 1);
    }

    static constexpr uint16_t pieceWeights[12] = {
            100, 100, 350, 350, 350, 350, 525, 525, 1000, 1000, 65535, 65535
    };

    inline uint64_t popcnt(uint64_t b) {
        return __builtin_popcountll(b);
    }

    inline int8_t bitscanForward(uint64_t b) {
        return __builtin_ctzll(b);
    }

    inline int8_t bitscanReverse(uint64_t b) {
        return 63 ^ __builtin_clzll(b);
    }

    inline int8_t popLsb(uint64_t &b) {
        int8_t lsb = bitscanForward(b);
        b &= b - 1;
        return lsb;
    }

    inline uint32_t encodeMove(Move* move) {
        return (move->promotionPiece << 20) | (move->piece << 15) |
               (move->to << 7) | move->from;
    }

    inline uint16_t getPieceWeight(PieceType type) {
        return pieceWeights[type];
    }

    inline int mvvlva(PieceType attacker, PieceType victim) {
        return MVVLVA_TABLE[attacker][victim];
    }

    std::string getNotation(int8_t square);

    int8_t getSquareFromString(std::string move);

    char getCharacterForPieceType(PieceType pieceType);
}