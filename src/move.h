
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
#include <array>
#include <string>
#include <string_view>
#include "constants.h"
#include "types.h"


namespace Zagreus {
using Move = uint16_t;

struct MoveList {
    std::array<Move, MAX_MOVES> moves{};
    uint8_t size = 0;

    MoveList() = default;
    MoveList(const MoveList&) = delete;
    MoveList& operator=(const MoveList&) = delete;
};

enum MoveType : uint8_t {
    NORMAL = 0b00,
    PROMOTION = 0b01,
    EN_PASSANT = 0b10,
    CASTLING = 0b11
};

enum PromotionPiece : uint8_t {
    QUEEN_PROMOTION = 0b00,
    ROOK_PROMOTION = 0b01,
    BISHOP_PROMOTION = 0b10,
    KNIGHT_PROMOTION = 0b11
};


/**
 * \brief Struct representing a PV line.
 */
struct PvLine {
    Move moves[MAX_MOVES]{};
    int moveCount = 0;
    int startPly = 0;

    explicit PvLine(int startPly) : startPly(startPly) {
        std::fill_n(moves, MAX_MOVES, NO_MOVE);
    }
};

std::string getMoveNotation(uint8_t fromSquare, uint8_t toSquare);

std::string getMoveNotation(uint8_t fromSquare, uint8_t toSquare, PromotionPiece promotionPiece);

std::string getMoveNotation(Move move);

Move getMoveFromMoveNotation(std::string_view notation);

std::string getSquareNotation(Square square);

Square getSquareFromNotation(std::string_view notation);

// bits 0-5: from square (0-63)
// bits 6-11: to square (0-63)
// bits 12-13: move type (00 = normal, 01 = promotion, 10 = en passant, 11 = castling)
// bits 14-15: promotion piece (00 = queen, 01 = rook, 10 = bishop, 11 = knight)
inline Move encodeMove(const uint8_t fromSquare, const uint8_t toSquare) {
    // Assume normal move, so bits 12-13 are 00 and bits 14-15 are 00
    return fromSquare | (toSquare << 6);
}

inline Move encodeMove(const uint8_t fromSquare, const uint8_t toSquare, const MoveType moveType) {
    return fromSquare | (toSquare << 6) | (moveType << 12);
}

inline Move encodeMove(const uint8_t fromSquare, const uint8_t toSquare, const PromotionPiece promotionPiece) {
    return fromSquare | (toSquare << 6) | (PROMOTION << 12) | (promotionPiece << 14);
}

inline Square getFromSquare(const Move move) {
    return static_cast<Square>(move & 0x3F);
}

inline Square getToSquare(const Move move) {
    return static_cast<Square>((move >> 6) & 0x3F);
}

inline MoveType getMoveType(const Move move) {
    return static_cast<MoveType>((move >> 12) & 0x3);
}

inline PromotionPiece getPromotionPiece(const Move move) {
    return static_cast<PromotionPiece>((move >> 14) & 0x3);
}

inline Piece getPieceFromPromotionPiece(const PromotionPiece promotionPiece, const PieceColor color) {
    switch (promotionPiece) {
        case QUEEN_PROMOTION:
            return static_cast<Piece>(WHITE_QUEEN + color);
        case ROOK_PROMOTION:
            return static_cast<Piece>(WHITE_ROOK + color);
        case BISHOP_PROMOTION:
            return static_cast<Piece>(WHITE_BISHOP + color);
        case KNIGHT_PROMOTION:
            return static_cast<Piece>(WHITE_KNIGHT + color);
        default:
            assert(false);
            return EMPTY;
    }
}

/**
 * \brief Parses the given pvLine as a string of moves.
 *
 * \return The string representation of the pvLine.
 */
inline std::string parsePvLine(const PvLine& pvLine) {
    std::string result;

    for (int i = 0; i < pvLine.moveCount; ++i) {
        result += getMoveNotation(pvLine.moves[i]) + " ";
    }

    // remove trailing space
    if (!result.empty()) {
        result.pop_back();
    }

    return result;
}
} // namespace Zagreus
