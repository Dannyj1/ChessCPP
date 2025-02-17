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

#include "bitwise.h"
#include "constants.h"
#include "types.h"

namespace Zagreus {

/**
 * \brief Initializes the attack lookup tables for pawns, knights, and kings.
 */
void initializeAttackLookupTables();

/**
 * \brief Initializes the between lookup table.
 */
void initializeBetweenLookupTable();

/**
 * \brief Shifts the bitboard north by one rank.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftNorth(const uint64_t bb) {
    return bb << 8;
}

/**
 * \brief Shifts the bitboard south by one rank.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftSouth(const uint64_t bb) {
    return bb >> 8;
}

/**
 * \brief Shifts the bitboard east by one file.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftEast(const uint64_t bb) {
    return (bb << 1) & NOT_A_FILE;
}

/**
 * \brief Shifts the bitboard west by one file.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftWest(const uint64_t bb) {
    return (bb >> 1) & NOT_H_FILE;
}

/**
 * \brief Shifts the bitboard north-east by one rank and one file.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftNorthEast(const uint64_t bb) {
    return (bb << 9) & NOT_A_FILE;
}

/**
 * \brief Shifts the bitboard north-west by one rank and one file.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftNorthWest(const uint64_t bb) {
    return (bb << 7) & NOT_H_FILE;
}

/**
 * \brief Shifts the bitboard south-east by one rank and one file.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftSouthEast(const uint64_t bb) {
    return (bb >> 7) & NOT_A_FILE;
}

/**
 * \brief Shifts the bitboard south-west by one rank and one file.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftSouthWest(const uint64_t bb) {
    return (bb >> 9) & NOT_H_FILE;
}

/**
 * \brief Shifts the bitboard north-north-east by two ranks and one file.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftNorthNorthEast(const uint64_t bb) {
    return (bb << 17) & NOT_A_FILE;
}

/**
 * \brief Shifts the bitboard north-east-east by one rank and two files.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftNorthEastEast(const uint64_t bb) {
    return (bb << 10) & NOT_AB_FILE;
}

/**
 * \brief Shifts the bitboard south-east-east by one rank and two files.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftSouthEastEast(const uint64_t bb) {
    return (bb >> 6) & NOT_AB_FILE;
}

/**
 * \brief Shifts the bitboard south-south-east by two ranks and one file.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftSouthSouthEast(const uint64_t bb) {
    return (bb >> 15) & NOT_A_FILE;
}

/**
 * \brief Shifts the bitboard north-north-west by two ranks and one file.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftNorthNorthWest(const uint64_t bb) {
    return (bb << 15) & NOT_H_FILE;
}

/**
 * \brief Shifts the bitboard north-west-west by one rank and two files.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftNorthWestWest(const uint64_t bb) {
    return (bb << 6) & NOT_GH_FILE;
}

/**
 * \brief Shifts the bitboard south-west-west by one rank and two files.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftSouthWestWest(const uint64_t bb) {
    return (bb >> 10) & NOT_GH_FILE;
}

/**
 * \brief Shifts the bitboard south-south-west by two ranks and one file.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
inline uint64_t shiftSouthSouthWest(const uint64_t bb) {
    return (bb >> 17) & NOT_H_FILE;
}

/**
 * \brief Shifts the bitboard in the specified direction.
 * \tparam direction The direction to shift.
 * \param bb The bitboard to shift.
 * \return The shifted bitboard.
 */
template <Direction direction>
constexpr uint64_t shift(const uint64_t bb) {
    switch (direction) {
        case NORTH:
            return shiftNorth(bb);
        case SOUTH:
            return shiftSouth(bb);
        case EAST:
            return shiftEast(bb);
        case WEST:
            return shiftWest(bb);
        case NORTH_EAST:
            return shiftNorthEast(bb);
        case NORTH_WEST:
            return shiftNorthWest(bb);
        case SOUTH_EAST:
            return shiftSouthEast(bb);
        case SOUTH_WEST:
            return shiftSouthWest(bb);
        case NORTH_NORTH_EAST:
            return shiftNorthNorthEast(bb);
        case NORTH_EAST_EAST:
            return shiftNorthEastEast(bb);
        case SOUTH_EAST_EAST:
            return shiftSouthEastEast(bb);
        case SOUTH_SOUTH_EAST:
            return shiftSouthSouthEast(bb);
        case SOUTH_SOUTH_WEST:
            return shiftSouthSouthWest(bb);
        case SOUTH_WEST_WEST:
            return shiftSouthWestWest(bb);
        default:
            assert(false);
            return bb;
    }
}

/**
 * \brief Calculates the single push for white pawns.
 * \param bb The bitboard representing the pawns.
 * \param empty The bitboard representing the empty squares.
 * \return The bitboard representing the single push.
 */
inline uint64_t whitePawnSinglePush(const uint64_t bb, const uint64_t empty) {
    return shiftNorth(bb) & empty;
}

/**
 * \brief Calculates the double push for white pawns.
 * \param bb The bitboard representing the pawns.
 * \param empty The bitboard representing the empty squares.
 * \return The bitboard representing the double push.
 */
inline uint64_t whitePawnDoublePush(const uint64_t bb, const uint64_t empty) {
    const uint64_t singlePush = whitePawnSinglePush(bb, empty);
    return shiftNorth(singlePush) & empty & RANK_4;
}

/**
 * \brief Calculates the west attacks for white pawns.
 * \param bb The bitboard representing the pawns.
 * \return The bitboard representing the west attacks.
 */
inline uint64_t whitePawnWestAttacks(const uint64_t bb) {
    return shiftNorthWest(bb);
}

/**
 * \brief Calculates the east attacks for white pawns.
 * \param bb The bitboard representing the pawns.
 * \return The bitboard representing the east attacks.
 */
inline uint64_t whitePawnEastAttacks(const uint64_t bb) {
    return shiftNorthEast(bb);
}

/**
 * \brief Calculates the attacks for white pawns.
 * \param bb The bitboard representing the pawns.
 * \return The bitboard representing the attacks.
 */
inline uint64_t calculateWhitePawnAttacks(const uint64_t bb) {
    return whitePawnWestAttacks(bb) | whitePawnEastAttacks(bb);
}

/**
 * \brief Calculates the pushable pawns for white.
 * \param bb The bitboard representing the pawns.
 * \param empty The bitboard representing the empty squares.
 * \return The bitboard representing the pushable pawns.
 */
inline uint64_t whitePushablePawns(const uint64_t bb, const uint64_t empty) {
    return shiftSouth(empty) & bb;
}

/**
 * \brief Calculates the double pushable pawns for white.
 * \param bb The bitboard representing the pawns.
 * \param empty The bitboard representing the empty squares.
 * \return The bitboard representing the double pushable pawns.
 */
inline uint64_t whiteDoublePushablePawns(const uint64_t bb, const uint64_t empty) {
    const uint64_t emptyRank3 = shiftSouth(empty & RANK_4) & empty;
    return whitePushablePawns(bb, emptyRank3);
}

/**
 * \brief Calculates the single push for black pawns.
 * \param bb The bitboard representing the pawns.
 * \param empty The bitboard representing the empty squares.
 * \return The bitboard representing the single push.
 */
inline uint64_t blackPawnSinglePush(const uint64_t bb, const uint64_t empty) {
    return shiftSouth(bb) & empty;
}

/**
 * \brief Calculates the double push for black pawns.
 * \param bb The bitboard representing the pawns.
 * \param empty The bitboard representing the empty squares.
 * \return The bitboard representing the double push.
 */
inline uint64_t blackPawnDoublePush(const uint64_t bb, const uint64_t empty) {
    const uint64_t singlePush = blackPawnSinglePush(bb, empty);
    return shiftSouth(singlePush) & empty & RANK_5;
}

/**
 * \brief Calculates the west attacks for black pawns.
 * \param bb The bitboard representing the pawns.
 * \return The bitboard representing the west attacks.
 */
inline uint64_t blackPawnWestAttacks(const uint64_t bb) {
    return shiftSouthWest(bb);
}

/**
 * \brief Calculates the east attacks for black pawns.
 * \param bb The bitboard representing the pawns.
 * \return The bitboard representing the east attacks.
 */
inline uint64_t blackPawnEastAttacks(const uint64_t bb) {
    return shiftSouthEast(bb);
}

/**
 * \brief Calculates the attacks for black pawns.
 * \param bb The bitboard representing the pawns.
 * \return The bitboard representing the attacks.
 */
inline uint64_t calculateBlackPawnAttacks(const uint64_t bb) {
    return blackPawnWestAttacks(bb) | blackPawnEastAttacks(bb);
}

/**
 * \brief Calculates the pushable pawns for black.
 * \param bb The bitboard representing the pawns.
 * \param empty The bitboard representing the empty squares.
 * \return The bitboard representing the pushable pawns.
 */
inline uint64_t blackPushablePawns(const uint64_t bb, const uint64_t empty) {
    return shiftNorth(empty) & bb;
}

/**
 * \brief Calculates the double pushable pawns for black.
 * \param bb The bitboard representing the pawns.
 * \param empty The bitboard representing the empty squares.
 * \return The bitboard representing the double pushable pawns.
 */
inline uint64_t blackDoublePushablePawns(const uint64_t bb, const uint64_t empty) {
    const uint64_t emptyRank6 = shiftNorth(empty & RANK_5) & empty;
    return blackPushablePawns(bb, emptyRank6);
}

/**
 * \brief Calculates the attacks for knights.
 * \param bb The bitboard representing the knights.
 * \return The bitboard representing the attacks.
 */
inline uint64_t calculateKnightAttacks(const uint64_t bb) {
    return shiftNorthNorthEast(bb) | shiftNorthEastEast(bb) | shiftSouthEastEast(bb) |
           shiftSouthSouthEast(bb) | shiftSouthSouthWest(bb) | shiftSouthWestWest(bb) |
           shiftNorthWestWest(bb) | shiftNorthNorthWest(bb);
}

/**
 * \brief Retrieves the pawn attacks for a given square and color.
 * \tparam color The color of the pawn (WHITE or BLACK).
 * \param square The square index (0-63).
 * \return A bitboard representing the attack pattern.
 */
template <PieceColor color>
uint64_t getPawnAttacks(uint8_t square);

/**
 * \brief Retrieves the knight attacks for a given square.
 * \param square The square index (0-63).
 * \return A bitboard representing the attack pattern.
 */
uint64_t getKnightAttacks(uint8_t square);

/**
 * \brief Retrieves the king attacks for a given square.
 * \param square The square index (0-63).
 * \return A bitboard representing the attack pattern.
 */
uint64_t getKingAttacks(uint8_t square);

/**
 * \brief Retrieves the bishop attacks for a given square and occupied squares.
 * \param square The square index (0-63).
 * \param occupied A bitboard representing the occupied squares.
 * \return A bitboard representing the attack pattern.
 */
uint64_t getBishopAttacks(uint8_t square, uint64_t occupied);

/**
 * \brief Retrieves the rook attacks for a given square and occupied squares.
 * \param square The square index (0-63).
 * \param occupied A bitboard representing the occupied squares.
 * \return A bitboard representing the attack pattern.
 */
uint64_t getRookAttacks(uint8_t square, uint64_t occupied);

/**
 * \brief Retrieves the queen attacks for a given square and occupied squares.
 * \param square The square index (0-63).
 * \param occupied A bitboard representing the occupied squares.
 * \return A bitboard representing the attack pattern.
 */
uint64_t queenAttacks(uint8_t square, uint64_t occupied);

/**
 * \brief Calculates the attacks for kings.
 * \param bb The bitboard representing the kings.
 * \return The bitboard representing the attacks.
 */
inline uint64_t calculateKingAttacks(uint64_t bb) {
    const uint64_t attacks = shiftEast(bb) | shiftWest(bb);
    bb |= attacks;
    return attacks | shiftNorth(bb) | shiftSouth(bb);
}

/**
 * \brief Converts a square index to a bitboard.
 * \param square The square index (0-63).
 * \return The bitboard representing the square.
 */
inline uint64_t squareToBitboard(const uint8_t square) {
    return 1ULL << square;
}

/**
 * \brief Converts a bitboard to a square index.
 * \param bb The bitboard to convert.
 * \return The square index.
 */
inline Square bitboardToSquare(const uint64_t bb) {
    if (bb == 0) {
        return A1;
    }

    return static_cast<Square>(bitscanForward(bb));
}

/**
 * \brief Gets a bitboard of all squares between two squares.
 * \param fromSquare The starting square.
 * \param toSquare The ending square.
 *
 * \return The bitboard of squares between the two squares.
 */
uint64_t getSquaresBetween(Square fromSquare, Square toSquare);
} // namespace Zagreus
