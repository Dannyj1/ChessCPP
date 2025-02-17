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

#include "timeman.h"
#include <algorithm>
#include <limits>

namespace Zagreus {
template <PieceColor color>
int calculateSearchTime(SearchParams& params) {
    int timeLeft = color == WHITE ? params.whiteTime : params.blackTime;
    int timeInc = color == WHITE ? params.whiteInc : params.blackInc;

    if (timeLeft > 0) {
        int movesToGo = 50;
        int searchTime = timeLeft + (timeInc * movesToGo);

        searchTime /= movesToGo;
        return std::max<int>(searchTime, 1);
    } else {
        return std::numeric_limits<int>::max();
    }
}

template int calculateSearchTime<WHITE>(SearchParams& params);
template int calculateSearchTime<BLACK>(SearchParams& params);

} // namespace Zagreus
