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

#include "board.h"
#include "move.h"

namespace Zagreus {
class MovePicker {
private:
    MoveList& moveList;
    std::array<int, MAX_MOVES> scores{};
    int currentIndex = 0;

public:
    explicit MovePicker(MoveList& moveList) : moveList(moveList) {
    }

    MovePicker(const MovePicker&) = delete;

    MovePicker& operator=(const MovePicker&) = delete;

    /**
     * \brief Checks if there is a next move and retrieves it.
     * \param[out] move The next move if available.
     * \return True if there is a next move, false otherwise.
     */
    [[nodiscard]] bool next(Move& move);

    /**
     * \brief Resets the move picker to the beginning of the move list.
     */
    void reset();

    /**
     * \brief sorts the move list based on certain criteria.
     * \param board The current board which is used to sort the moves.
     */
    void score(Board& board);

};
} // namespace Zagreus
