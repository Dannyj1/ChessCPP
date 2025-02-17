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

#include "catch2/catch_test_macros.hpp"

#include "../src/types.h"

namespace Zagreus {
TEST_CASE("test_pieceType", "[types]") {
    REQUIRE(getPieceType(Piece::WHITE_PAWN) == PieceType::PAWN);
    REQUIRE(getPieceType(Piece::BLACK_PAWN) == PieceType::PAWN);
    REQUIRE(getPieceType(Piece::WHITE_KNIGHT) == PieceType::KNIGHT);
    REQUIRE(getPieceType(Piece::BLACK_KNIGHT) == PieceType::KNIGHT);
    REQUIRE(getPieceType(Piece::WHITE_BISHOP) == PieceType::BISHOP);
    REQUIRE(getPieceType(Piece::BLACK_BISHOP) == PieceType::BISHOP);
    REQUIRE(getPieceType(Piece::WHITE_ROOK) == PieceType::ROOK);
    REQUIRE(getPieceType(Piece::BLACK_ROOK) == PieceType::ROOK);
    REQUIRE(getPieceType(Piece::WHITE_QUEEN) == PieceType::QUEEN);
    REQUIRE(getPieceType(Piece::BLACK_QUEEN) == PieceType::QUEEN);
    REQUIRE(getPieceType(Piece::WHITE_KING) == PieceType::KING);
    REQUIRE(getPieceType(Piece::BLACK_KING) == PieceType::KING);
}

TEST_CASE("test_pieceColor", "[types]") {
    const std::vector pieces = {
        WHITE_PAWN, BLACK_PAWN,
        WHITE_KNIGHT, BLACK_KNIGHT,
        WHITE_BISHOP, BLACK_BISHOP,
        WHITE_ROOK, BLACK_ROOK,
        WHITE_QUEEN, BLACK_QUEEN,
        WHITE_KING, BLACK_KING
    };

    for (const auto& piece : pieces) {
        PieceColor color = getPieceColor(piece);
        if (piece == WHITE_PAWN || piece == WHITE_KNIGHT || piece == WHITE_BISHOP ||
            piece == WHITE_ROOK || piece == WHITE_QUEEN || piece == WHITE_KING) {
            REQUIRE(color == PieceColor::WHITE);
            } else {
                REQUIRE(color == PieceColor::BLACK);
            }
    }
}

TEST_CASE("test_getPieceFromType", "[types]") {
    REQUIRE(getPieceFromType(PieceType::PAWN, PieceColor::WHITE) == Piece::WHITE_PAWN);
    REQUIRE(getPieceFromType(PieceType::PAWN, PieceColor::BLACK) == Piece::BLACK_PAWN);

    REQUIRE(getPieceFromType(PieceType::KNIGHT, PieceColor::WHITE) == Piece::WHITE_KNIGHT);
    REQUIRE(getPieceFromType(PieceType::KNIGHT, PieceColor::BLACK) == Piece::BLACK_KNIGHT);

    REQUIRE(getPieceFromType(PieceType::BISHOP, PieceColor::WHITE) == Piece::WHITE_BISHOP);
    REQUIRE(getPieceFromType(PieceType::BISHOP, PieceColor::BLACK) == Piece::BLACK_BISHOP);

    REQUIRE(getPieceFromType(PieceType::ROOK, PieceColor::WHITE) == Piece::WHITE_ROOK);
    REQUIRE(getPieceFromType(PieceType::ROOK, PieceColor::BLACK) == Piece::BLACK_ROOK);

    REQUIRE(getPieceFromType(PieceType::QUEEN, PieceColor::WHITE) == Piece::WHITE_QUEEN);
    REQUIRE(getPieceFromType(PieceType::QUEEN, PieceColor::BLACK) == Piece::BLACK_QUEEN);

    REQUIRE(getPieceFromType(PieceType::KING, PieceColor::WHITE) == Piece::WHITE_KING);
    REQUIRE(getPieceFromType(PieceType::KING, PieceColor::BLACK) == Piece::BLACK_KING);
}
} // namespace Zagreus
