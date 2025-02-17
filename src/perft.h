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

namespace Zagreus {
/**
 * \brief Performs a perft test on the given board to a specified depth.
 *
 * This function recursively generates all possible moves up to a given depth and counts the number of nodes reached.
 *
 * \param board The board object on which to perform the perft.
 * \param depth The depth to which moves should be generated.
 * \param printNodes If true, prints the number of nodes for each move at the root level.
 * \return The number of nodes reached.
 */
uint64_t perft(Board &board, int depth, bool printNodes = true);
} // namespace Zagreus