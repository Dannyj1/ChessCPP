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

#include <cassert>

#include "move.h"

namespace Zagreus {
/**
 * \brief Converts the from and to squares from the given move to chess notation (e.g. e2e4).
 * \param fromSquare The starting square of the move.
 * \param toSquare The ending square of the move.
 * \return A string representing the move in chess notation.
 */
std::string getMoveNotation(const uint8_t fromSquare, const uint8_t toSquare) {
    assert(fromSquare < SQUARES);
    assert(toSquare < SQUARES);
    std::string notation;

    notation += static_cast<char>(fromSquare % 8 + 'a');
    notation += static_cast<char>(fromSquare / 8 + '1');
    notation += static_cast<char>(toSquare % 8 + 'a');
    notation += static_cast<char>(toSquare / 8 + '1');

    assert(notation.size() == 4);
    return notation;
}

/**
 * \brief Converts the from and to squares from the given move to chess notation with promotion (e.g. e7e8q).
 * \param fromSquare The starting square of the move.
 * \param toSquare The ending square of the move.
 * \param promotionPiece The piece to which the pawn is promoted.
 * \return A string representing the move in chess notation with promotion.
 */
std::string getMoveNotation(const uint8_t fromSquare, const uint8_t toSquare, const PromotionPiece promotionPiece) {
    assert(fromSquare < SQUARES);
    assert(toSquare < SQUARES);
    assert(promotionPiece < 4);
    std::string notation;

    notation += static_cast<char>(fromSquare % 8 + 'a');
    notation += static_cast<char>(fromSquare / 8 + '1');
    notation += static_cast<char>(toSquare % 8 + 'a');
    notation += static_cast<char>(toSquare / 8 + '1');
    notation += "qrbn"[promotionPiece];

    assert(notation.size() == 5);
    return notation;
}

/**
 * \brief Converts a Move object to chess notation (e.g. e2e4).
 * \param move The move to be converted.
 * \return A string representing the move in chess notation.
 */
std::string getMoveNotation(const Move move) {
    const uint8_t fromSquare = getFromSquare(move);
    const uint8_t toSquare = getToSquare(move);

    if (const MoveType moveType = getMoveType(move);
        moveType == PROMOTION) {
        const PromotionPiece promotionPiece = getPromotionPiece(move);

        return getMoveNotation(fromSquare, toSquare, promotionPiece);
    }

    return getMoveNotation(fromSquare, toSquare);
}

/**
 * \brief Converts a string in chess notation to a Move object.
 * \param notation The string representing the move in chess notation.
 * \return A Move object representing the move.
 */
Move getMoveFromMoveNotation(const std::string_view notation) {
    const Square fromSquare = getSquareFromNotation(notation.substr(0, 2));
    const Square toSquare = getSquareFromNotation(notation.substr(2, 2));

    // If there is a 5th character, it is a promotion
    if (notation.size() == 5) {
        const char promotionPieceChar = notation[4];
        PromotionPiece promotionPiece;

        switch (promotionPieceChar) {
            case 'q':
                promotionPiece = QUEEN_PROMOTION;
                break;
            case 'r':
                promotionPiece = ROOK_PROMOTION;
                break;
            case 'b':
                promotionPiece = BISHOP_PROMOTION;
                break;
            case 'n':
                promotionPiece = KNIGHT_PROMOTION;
                break;
            default:
                assert(false);
        }

        return encodeMove(fromSquare, toSquare, promotionPiece);
    }

    return encodeMove(fromSquare, toSquare);
}

/**
 * \brief Converts a square to its chess notation (e.g. e2).
 * \param square The square to be converted.
 * \return A string representing the square in chess notation.
 */
std::string getSquareNotation(const Square square) {
    assert(square < SQUARES);
    std::string notation;

    notation += static_cast<char>(square % 8 + 'a');
    notation += static_cast<char>(square / 8 + '1');

    assert(notation.size() == 2);
    return notation;
}

/**
 * \brief Converts a string in chess notation to a square.
 * \param notation The string representing the square in chess notation.
 * \return A Square integer representing the square.
 */
Square getSquareFromNotation(const std::string_view notation) {
    const uint8_t file = notation[0] - 'a';
    const uint8_t rank = notation[1] - '1';

    return static_cast<Square>(file + rank * 8);
}
} // namespace Zagreus
