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

#include <catch2/catch_test_macros.hpp>

#include "../src/board.h"
#include "../src/bitboard.h"
#include "../src/perft.h"
#include "../src/uci.h"
#include "../src/magics.h"

namespace Zagreus {
// FEN string, depth, expected nodes
static const std::vector<std::tuple<std::string, int, int>> POSITIONS{
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4, 197281},
    {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 3, 97862},
    {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 5, 674624},
    {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 4, 422333},
    {"r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 4, 422333},
    {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 3, 62379},
    {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 3, 89890},
};

TEST_CASE("test_Perft", "[perft]") {
    Engine engine{};

    initZobristConstants();
    initializeMagicBitboards();
    initializeAttackLookupTables();

    for (const auto& [fen, depth, expectedNodes] : POSITIONS) {
        Board board{};

        board.setFromFEN(fen);

        int actualNodes = perft(board, depth, false);

        CAPTURE(fen, depth, expectedNodes, actualNodes);
        REQUIRE(actualNodes == expectedNodes);
    }
}
} // namespace Zagreus
