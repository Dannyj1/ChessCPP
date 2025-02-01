
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

#include <chrono>
#include <cstdint>

#include "search.h"

namespace Zagreus {
enum TTNodeType : uint8_t {
    EXACT, // PV
    ALPHA,
    BETA
};

struct TTEntry {
    uint32_t validationHash = 0;
    int16_t score = 0;
    Move bestMove = 0;
    int8_t depth = INT8_MIN;
    TTNodeType nodeType = EXACT;
};

class TranspositionTable {
public:
    TTEntry* transpositionTable = new TTEntry[1]{};
    uint64_t hashSize = 0;

    ~TranspositionTable() {
        delete[] transpositionTable;
    }

    void reset() {
        std::fill_n(transpositionTable, hashSize, TTEntry{});
    }

    TranspositionTable(TranspositionTable& other) = delete;
    TranspositionTable() = default;

    void operator=(const TranspositionTable&) = delete;

    static TranspositionTable* getTT();

    void setTableSize(int megaBytes);

    void savePosition(uint64_t zobristHash, int8_t depth, int ply, int score, Move bestMove,
                      TTNodeType nodeType) const;

    [[nodiscard]] int16_t probePosition(uint64_t zobristHash, int8_t depth, int alpha, int beta, int ply) const;

    [[nodiscard]] TTEntry* getEntry(uint64_t zobristHash) const;
};
} // namespace Zagreus
