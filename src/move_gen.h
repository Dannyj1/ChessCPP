
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

#include <cstdint>
#include "board.h"
#include "move.h"
#include "types.h"

namespace Zagreus {
enum GenerationType : uint8_t {
    ALL,
    QSEARCH,
    EVASIONS
};

/**
 * \brief Generates all pseudo-legal moves for all pieces of a certain color for a given color and generation type.
 * \tparam color The color of the pieces to generate moves for.
 * \tparam type The type of moves to generate (e.g., all moves, captures, quiet moves).
 * \param board The board object for which to generate moves.
 * \param[out] moves The list to store the generated moves.
 */
template <PieceColor color, GenerationType type>
void generateMoves(const Board& board, MoveList& moves);

/**
 * \brief Generates all pseudo-legal pawn moves for a given color and generation type.
 * \tparam color The color of the pawns to generate moves for.
 * \tparam type The type of moves to generate (e.g., all moves, captures, quiet moves).
 * \param board The board object for which to generate moves.
 * \param[out] moves The list to store the generated moves.
 * \param genMask The mask to filter out invalid moves.
 */
template <PieceColor color, GenerationType type>
void generatePawnMoves(const Board& board, MoveList& moves, uint64_t genMask);

/**
 * \brief Generates all pseudo-legal knight moves for a given color and generation type.
 * \tparam color The color of the knights to generate moves for.
 * \tparam type The type of moves to generate (e.g., all moves, captures, quiet moves).
 * \param board The board object for which to generate moves.
 * \param[out] moves The list to store the generated moves.
 * \param genMask The mask to filter out invalid moves.
 */
template <PieceColor color, GenerationType type>
void generateKnightMoves(const Board& board, MoveList& moves, uint64_t genMask);

/**
 * \brief Generates all pseudo-legal bishop moves for a given color and generation type.
 * \tparam color The color of the bishops to generate moves for.
 * \tparam type The type of moves to generate (e.g., all moves, captures, quiet moves).
 * \param board The board object for which to generate moves.
 * \param[out] moves The list to store the generated moves.
 * \param genMask The mask to filter out invalid moves.
 */
template <PieceColor color, GenerationType type>
void generateBishopMoves(const Board& board, MoveList& moves, uint64_t genMask);

/**
 * \brief Generates all pseudo-legal rook moves for a given color and generation type.
 * \tparam color The color of the rooks to generate moves for.
 * \tparam type The type of moves to generate (e.g., all moves, captures, quiet moves).
 * \param board The board object for which to generate moves.
 * \param[out] moves The list to store the generated moves.
 * \param genMask The mask to filter out invalid moves.
 */
template <PieceColor color, GenerationType type>
void generateRookMoves(const Board& board, MoveList& moves, uint64_t genMask);

/**
 * \brief Generates all pseudo-legal queen moves for a given color and generation type.
 * \tparam color The color of the queens to generate moves for.
 * \tparam type The type of moves to generate (e.g., all moves, captures, quiet moves).
 * \param board The board object for which to generate moves.
 * \param[out] moves The list to store the generated moves.
 * \param genMask The mask to filter out invalid moves.
 */
template <PieceColor color, GenerationType type>
void generateQueenMoves(const Board& board, MoveList& moves, uint64_t genMask);

/**
 * \brief Generates all pseudo-legal king moves for a given color and generation type.
 * \tparam color The color of the king to generate moves for.
 * \tparam type The type of moves to generate (e.g., all moves, captures, quiet moves).
 * \param board The board object for which to generate moves.
 * \param[out] moves The list to store the generated moves.
 * \param genMask The mask to filter out invalid moves.
 */
template <PieceColor color, GenerationType type>
void generateKingMoves(const Board& board, MoveList& moves, uint64_t genMask);
} // namespace Zagreus
