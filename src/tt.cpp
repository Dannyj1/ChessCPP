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

#include "tt.h"
#include <cmath>
#include "constants.h"

namespace Zagreus {
void TranspositionTable::savePosition(const uint64_t zobristHash, const int8_t depth, const int ply, int score,
                                      const Move bestMove, const TTNodeType nodeType) const {
    const uint64_t index = zobristHash & hashSize;
    TTEntry* entry = &transpositionTable[index];

    // Only replace the entry if:
    // 1. Validation hash is 0 (the entry is empty)
    // 1. Depth > 0 (the new node is a pvSearch node, can replace any node)
    // 2. The entries' depth < 0 (the entry is a qSearch node, can be replaced by any node)
    if (entry->validationHash == 0 || depth > 0 || entry->depth < 0) {
        if (score >= (MATE_SCORE - MAX_PLIES)) {
            score += ply;
        } else if (score <= (-MATE_SCORE + MAX_PLIES)) {
            score -= ply;
        }

        score = std::clamp(score, INT16_MIN, INT16_MAX);

        entry->validationHash = zobristHash >> 32;
        entry->depth = depth;
        entry->bestMove = bestMove;
        entry->score = score;
        entry->nodeType = nodeType;
    }
}

int16_t TranspositionTable::probePosition(const uint64_t zobristHash, const int8_t depth, const int alpha,
                                          const int beta, const int ply) const {
    const uint64_t index = zobristHash & hashSize;
    const uint32_t validationHash = zobristHash >> 32;
    const TTEntry* entry = &transpositionTable[index];

    if (entry->validationHash == validationHash && entry->depth >= depth) {
        bool returnScore = false;

        if (entry->nodeType == EXACT) {
            returnScore = true;
        } else if (entry->nodeType == ALPHA) {
            if (entry->score <= alpha) {
                returnScore = true;
            }
        } else if (entry->nodeType == BETA) {
            if (entry->score >= beta) {
                returnScore = true;
            }
        }

        if (returnScore) {
            int adjustedScore = entry->score;

            if (adjustedScore >= MATE_SCORE) {
                adjustedScore -= ply;
            } else if (adjustedScore <= -MATE_SCORE) {
                adjustedScore += ply;
            }

            return adjustedScore;
        }
    }

    return NO_TT_SCORE;
}

TTEntry* TranspositionTable::getEntry(const uint64_t zobristHash) const {
    const uint64_t index = zobristHash & hashSize;
    TTEntry* entry = &transpositionTable[index];

    // Check validation hash to avoid hash collisions
    if (entry->validationHash == (zobristHash >> 32)) {
        return entry;
    }

    return nullptr;
}

void TranspositionTable::setTableSize(int megaBytes) {
    if ((megaBytes & (megaBytes - 1)) != 0) {
        megaBytes = 1 << static_cast<int>(log2(megaBytes));
    }

    const uint64_t byteSize = megaBytes * 1024 * 1024;
    const uint64_t entryCount = byteSize / sizeof(TTEntry);

    delete[] transpositionTable;
    transpositionTable = new TTEntry[entryCount]{};
    hashSize = entryCount - 1;

    std::fill_n(transpositionTable, entryCount, TTEntry{});
}

TranspositionTable* TranspositionTable::getTT() {
    static TranspositionTable instance{};
    return &instance;
}

template <PieceColor color>
void TranspositionTable::updateHistory(const Move move, const int value) {
    const Square from = getFromSquare(move);
    const Square to = getToSquare(move);
    const int clampedValue = std::clamp(value, -MAX_HISTORY, MAX_HISTORY);

    history[color][from][to] += clampedValue - history[color][from][to] * std::abs(clampedValue) / MAX_HISTORY;
}

template void TranspositionTable::updateHistory<WHITE>(Move move, int value);
template void TranspositionTable::updateHistory<BLACK>(Move move, int value);
} // namespace Zagreus


