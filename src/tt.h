
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

#include <algorithm>
#include <cstdint>
#include "move.h"

namespace Zagreus {
enum TTNodeType : uint8_t {
    EXACT, // PV
    ALPHA,
    BETA
};

struct TTEntry {
    uint32_t validationHash = 0;
    int16_t score = 0;
    Move bestMove = NO_MOVE;
    int8_t depth = INT8_MIN;
    TTNodeType nodeType = EXACT;
};

class TranspositionTable {
private:
    int history[COLORS][SQUARES][SQUARES]{};

public:
    TTEntry* transpositionTable = new TTEntry[1]{};
    uint64_t hashSize = 0;

    TranspositionTable() = default;

    ~TranspositionTable() {
        delete[] transpositionTable;
    }

    void reset() {
        std::fill_n(transpositionTable, hashSize + 1, TTEntry{});

        for (int color = 0; color < COLORS; color++) {
            for (int fromSquare = 0; fromSquare < SQUARES; fromSquare++) {
                std::fill_n(history[color][fromSquare], SQUARES, 0);
            }
        }
    }

    TranspositionTable(TranspositionTable& other) = delete;
    void operator=(const TranspositionTable&) = delete;

    static TranspositionTable* getTT();

    void setTableSize(int megaBytes);

    void savePosition(uint64_t zobristHash, int8_t depth, int ply, int score, Move bestMove,
                      TTNodeType nodeType) const;

    [[nodiscard]] int16_t probePosition(uint64_t zobristHash, int8_t depth, int alpha, int beta, int ply) const;

    [[nodiscard]] TTEntry* getEntry(uint64_t zobristHash) const;

    template <PieceColor color>
    void updateHistory(Move move, int value);

    template <PieceColor color>
    [[nodiscard]] int getHistoryValue(const Move move) const {
        const Square fromSquare = getFromSquare(move);
        const Square toSquare = getToSquare(move);

        return history[color][fromSquare][toSquare];
    }

    [[nodiscard]] int getHistoryValue(const PieceColor color, const Move move) const {
        const Square fromSquare = getFromSquare(move);
        const Square toSquare = getToSquare(move);

        return history[color][fromSquare][toSquare];
    }
};
} // namespace Zagreus
