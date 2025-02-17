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

namespace Zagreus {
/**
 * \brief Enum representing directions on the chessboard.
 */
enum Direction {
    NORTH = 8,
    SOUTH = -8,
    EAST = 1,
    WEST = -1,
    NORTH_EAST = 9,
    NORTH_WEST = 7,
    SOUTH_EAST = -7,
    SOUTH_WEST = -9,
    NORTH_NORTH_EAST = 17,
    NORTH_EAST_EAST = 10,
    SOUTH_EAST_EAST = -6,
    SOUTH_SOUTH_EAST = -15,
    SOUTH_SOUTH_WEST = -17,
    SOUTH_WEST_WEST = -10,
    NORTH_WEST_WEST = -17,
    NORTH_NORTH_WEST = -15
};

// clang-format off
/**
 * \brief Enum representing squares on the chessboard.
 */
enum Square : uint8_t {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NONE = 255
};

inline Square operator++(Square& piece, int) {
    const Square old = piece;
    piece = static_cast<Square>(piece + 1);
    return old;
}

inline Square operator--(Square& piece, int) {
    const Square old = piece;
    piece = static_cast<Square>(piece - 1);
    return old;
}

inline Square operator^(const Square square1, const int num) {
    return static_cast<Square>(static_cast<int>(square1) ^ num);
}

// clang-format on

/**
 * \brief Enum representing types of chess pieces.
 */
enum PieceType : uint8_t { PAWN = 0, KNIGHT = 1, BISHOP = 2, ROOK = 3, QUEEN = 4, KING = 5 };

inline PieceType operator++(PieceType& piece, int) {
    const PieceType old = piece;
    piece = static_cast<PieceType>(piece + 1);
    return old;
}

inline PieceType operator--(PieceType& piece, int) {
    const PieceType old = piece;
    piece = static_cast<PieceType>(piece - 1);
    return old;
}

/**
 * \brief Enum representing colors of chess pieces.
 */
enum PieceColor : uint8_t { WHITE = 0, BLACK = 1 };

/**
 * \brief Operator to get the opposite color.
 * \param color The current color.
 * \return The opposite color.
 */
constexpr PieceColor operator!(const PieceColor color) {
    return static_cast<PieceColor>(color ^ 1);
}

/**
 * \brief Enum representing chess pieces, separated by color and type.
 */
enum Piece : uint8_t {
    WHITE_PAWN,
    BLACK_PAWN,
    WHITE_KNIGHT,
    BLACK_KNIGHT,
    WHITE_BISHOP,
    BLACK_BISHOP,
    WHITE_ROOK,
    BLACK_ROOK,
    WHITE_QUEEN,
    BLACK_QUEEN,
    WHITE_KING,
    BLACK_KING,
    EMPTY = 255
};

inline Piece operator++(Piece& piece, int) {
    const Piece old = piece;
    piece = static_cast<Piece>(piece + 1);
    return old;
}

inline Piece operator--(Piece& piece, int) {
    const Piece old = piece;
    piece = static_cast<Piece>(piece - 1);
    return old;
}

/**
 * \brief Enum representing castling rights.
 */
enum CastlingRights : uint8_t {
    WHITE_KINGSIDE = 0b00000001,
    WHITE_QUEENSIDE = 0b00000010,
    BLACK_KINGSIDE = 0b00000100,
    BLACK_QUEENSIDE = 0b00001000,
    WHITE_CASTLING = 0b00000011,
    BLACK_CASTLING = 0b00001100,
};

/**
 * \brief Gets the color of a piece.
 * \param piece The piece.
 * \return The color of the piece.
 */
constexpr PieceColor getPieceColor(const Piece piece) {
    assert(piece != Piece::EMPTY);
    return static_cast<PieceColor>(piece & 1);
}

/**
 * \brief Gets the type of a piece.
 * \param piece The piece.
 * \return The type of the piece.
 */
constexpr PieceType getPieceType(const Piece piece) {
    assert(piece != Piece::EMPTY);
    return static_cast<PieceType>(piece >> 1);
}

/**
 * \brief Gets the piece type from a color and type.
 * \param type The type of the piece.
 * \param color The color of the piece.
 * \return The piece.
 */
constexpr Piece getPieceFromType(const PieceType type, const PieceColor color) {
    return static_cast<Piece>(color | (type << 1));
}

/**
 * \brief Gets the character representation of a piece.
 * \param piece The piece.
 * \return The character representing the piece.
 */
constexpr char getCharacterForPieceType(const Piece piece) {
    switch (piece) {
        case WHITE_PAWN:
            return 'P';
        case BLACK_PAWN:
            return 'p';
        case WHITE_KNIGHT:
            return 'N';
        case BLACK_KNIGHT:
            return 'n';
        case WHITE_BISHOP:
            return 'B';
        case BLACK_BISHOP:
            return 'b';
        case WHITE_ROOK:
            return 'R';
        case BLACK_ROOK:
            return 'r';
        case WHITE_QUEEN:
            return 'Q';
        case BLACK_QUEEN:
            return 'q';
        case WHITE_KING:
            return 'K';
        case BLACK_KING:
            return 'k';
        case EMPTY:
            return ' ';
    }

    return ' ';
}

/**
 * \brief Determines if a given piece type is a sliding piece.
 * \param pieceType The piece type.
 *
 * \return True if the piece type is a sliding piece, false otherwise.
 */

inline bool isSlidingPiece(const PieceType pieceType) {
    return pieceType == BISHOP || pieceType == ROOK || pieceType == QUEEN;
}
} // namespace Zagreus
