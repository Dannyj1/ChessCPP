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

#include <cstdint>
#include <chrono>
#include <exception>
#include <iostream>
#include <ratio>
#include <string>
#include <vector>

#include "board.h"
#include "search.h"
#include "tt.h"
#include "tuner.h"
#include "types.h"
#include "uci.h"

using namespace Zagreus;

// Some of these benchmark positions are taken from Stockfish's benchmark.cpp:
// https://github.com/official-stockfish/Stockfish/blob/master/src/benchmark.cpp
const std::vector<std::string> BENCHMARK_POSITIONS = {
    "8/8/1n3k2/8/3P3P/5K2/8/1N4Q1 w - -",
    "1rb1kbnr/p1q1pppp/np4B1/2pp4/P1PP4/2N1P3/1P3PPP/R1BQK1NR w Kk",
    "1rbk1bnr/pp1p1ppp/n1pq4/4pP1Q/P1B1P3/2P4P/1P1P2P1/RNB2KNR b - -",
    "r1bn1r2/2k3p1/6p1/pP3p2/4P2P/1P1P3R/2P1NB2/Q3K1n1 b - -",
    "1rb1kb1r/p3p1pp/n4p2/1pppNn2/4P3/2PP3P/PB4P1/RN1QKB1R b KQk -",
    "8/3P4/p4k2/P2Q4/N7/7B/4p3/4K3 w - -",
    "1B1N3k/4r3/5b1P/2n1P3/3K2p1/6P1/4b3/4R3 b - ",
    "r1n1k1r1/1pb2p2/5P2/pPPp2p1/5p1R/P1N1N3/2P1P3/B1R1KB2 w - -",
    "8/8/1qB1k3/1p6/3p4/1p6/8/4K3 b - -",
    "6nr/1pq1b1k1/1N6/5Ppp/pp2B1P1/B1P4P/P2pK3/3R2NR b - -",
    "rnbqk2r/1p1p1pbp/4p2n/p1p3p1/2PPP1P1/5P1N/PP1K2BP/RNBQR3 w kq -",
    "2bq1b2/4k3/1p3ppr/2pp1Q1N/3p3P/2P5/PpN2PP1/1RB1KBR1 w - -",
    "8/2k5/4p1B1/4P2p/4P2P/1K6/7N/1q6 w - -",
    "1N1k4/P5b1/4p1p1/2PP3p/R4BrP/8/2nKb2R/1r6 w - -",
    "2kN1r2/2n5/2Q5/7p/7P/1K1RB3/8/8 w - -",
    "8/7b/p1P1Pn2/P1k3N1/7p/K1p5/4B1P1/3N2R1 w - -",
    "rnbqkbnr/1p2pp1p/3p4/6p1/P1p2PP1/1p1P4/2P1P2P/RNBQKBNR w Kkq -",
    "1n6/8/2k3K1/2p5/2PpP1nb/3P4/8/1b6 w - -",
    "8/2R5/8/k7/N5PP/2K5/8/5b2 w - -",
    "r3k2r/1b1nb2p/p1p3pn/3Np3/1PPp1B2/7B/P2KPq2/RQ4NR w kq -",
    "1n6/2B2nK1/2k5/2p5/2Pp4/8/8/5b2 b - -",
    "3Kb3/8/8/P1R5/8/8/8/7k w - -",
    "1B1k2n1/6b1/r4npq/5p1r/pP2PP1P/2P5/R2NR3/3K4 b - -",
    "rnb4r/4b2k/1R1pP3/4N2P/pP2nP1P/p1N5/3B4/1K5R b - -",
    "8/1pp5/3R2P1/1pkn1p1p/5P2/r6b/8/1N2K1N1 b - -",
    "rn3bnr/pBp1pk1p/5p2/1p4p1/6b1/N3Q3/PP1P1PPP/R1B1K1NR b - -",
    "8/8/P7/2k4n/1pR4n/1B5P/3R4/3K2N1 b - -",
    "q7/p1rbn1p1/P1ppR3/1PP2P1k/1B1PN2p/1Q5P/1R2K3/8 b - -",
    "6r1/3k2N1/2n5/3pbP2/8/8/8/3K1n2 b - -",
    "bnk4r/7p/R3q3/p1Pp4/P7/3p4/3P2Kn/1NB5 w - -",
    "r7/n3k1b1/p2p4/P1pP4/2K5/P6N/3BB2R/6Q1 b - -",
    "3k4/8/8/1NP5/6B1/8/8/K7 b - -",
    "3nb3/5k2/4p3/4P2p/Qp2P2P/8/3K2B1/2N5 w - -",
    "6b1/k7/2KbR3/8/7P/8/8/8 b - -",
    "rnb5/4b1kr/1R1pP3/4N2P/pP2nP1P/p1N5/8/1KB4R w - -",
    "4r1k1/r1q2ppp/ppp2n2/4P3/5Rb1/1N1BQ3/PPP3PP/R5K1 w - - 1 17",
    "2rqkb1r/ppp2p2/2npb1p1/1N1Nn2p/2P1PP2/8/PP2B1PP/R1BQK2R b KQ - 0 11",
    "r1bbk1nr/pp3p1p/2n5/1N4p1/2Np1B2/8/PPP2PPP/2KR1B1R w kq - 0 13",
    "6k1/3b3r/1p1p4/p1n2p2/1PPNpP1q/P3Q1p1/1R1RB1P1/5K2 b - - 0 1",
    "r2r1n2/pp2bk2/2p1p2p/3q4/3PN1QP/2P3R1/P4PP1/5RK1 w - - 0 1",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 10",
    "8/8/8/8/8/6k1/6p1/6K1 w - -",
    "7k/7P/6K1/8/3B4/8/8/8 b - -",
    "8/8/8/8/5kp1/P7/8/1K1N4 w - - 0 1",
    "8/3k4/8/8/8/4B3/4KB2/2B5 w - - 0 1",
    "8/8/1P6/5pr1/8/4R3/7k/2K5 w - - 0 1",
    "8/8/3P3k/8/1p6/8/1P6/1K3n2 b - - 0 1",
    "8/R7/2q5/8/6k1/8/1P5p/K6R w - - 0 124"
};

const std::vector<std::string> FAST_BENCHMARK_POSITIONS = {
    "8/3k4/8/8/8/4B3/4KB2/2B5 w - - 0 1",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 10",
    "rn3bnr/pBp1pk1p/5p2/1p4p1/6b1/N3Q3/PP1P1PPP/R1B1K1NR b - -",
    "8/7b/p1P1Pn2/P1k3N1/7p/K1p5/4B1P1/3N2R1 w - -",
    "r1bn1r2/2k3p1/6p1/pP3p2/4P2P/1P1P3R/2P1NB2/Q3K1n1 b - -",
    "1n6/8/2k3K1/2p5/2PpP1nb/3P4/8/1b6 w - -",
    "rnbqkbnr/1p2pp1p/3p4/6p1/P1p2PP1/1p1P4/2P1P2P/RNBQKBNR w Kkq -",
    "1rb1kbnr/p1q1pppp/np4B1/2pp4/P1PP4/2N1P3/1P3PPP/R1BQK1NR w Kk",
    "r1bbk1nr/pp3p1p/2n5/1N4p1/2Np1B2/8/PPP2PPP/2KR1B1R w kq - 0 13",
    "8/2R5/8/k7/N5PP/2K5/8/5b2 w - -",
    "rnbqk2r/1p1p1pbp/4p2n/p1p3p1/2PPP1P1/5P1N/PP1K2BP/RNBQR3 w kq -",
    "8/R7/2q5/8/6k1/8/1P5p/K6R w - - 0 124"
};

void benchmark(bool fast);

int main(const int argc, char* argv[]) {
    if (argc > 1) {
        if (std::string(argv[1]) == "bench") {
            const bool fast = argc > 2 && std::string(argv[2]) == "fast";
            benchmark(fast);
            return 0;
        }

#ifdef ZAGREUS_TUNER
        if (std::string(argv[1]) == "tune") {
            const std::string filePath = argc > 2 ? std::string(argv[2]) : "";

            if (filePath.empty()) {
                std::cerr << "No file path provided for tuning" << std::endl;
                return 1;
            }

            startTuning(filePath);
            return 0;
        }
#endif
    }

    try {
        Engine engine;
        engine.startUci();
    } catch (const std::exception& e) {
        // Handle the exception or log the error
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

void benchmark(bool fast) {
    Engine engine{};
    uint64_t nodes = 0;
    double totalMs = 0;
    Board board{};

    engine.registerOptions();
    engine.doSetup();
    TranspositionTable::getTT()->setTableSize(128);
    std::vector<std::string> positions = fast ? FAST_BENCHMARK_POSITIONS : BENCHMARK_POSITIONS;
    SearchParams params{};

    params.depth = fast ? 4 : 8;

    for (const std::string& position : positions) {
        for (int i = 0; i < 2; i++) {
            TranspositionTable::getTT()->reset();
            const PieceColor color = i == 0 ? WHITE : BLACK;

            board.setFromFEN(position);
            board.setSideToMove(color);
            SearchStats stats{};
            auto start = std::chrono::steady_clock::now();

            if (color == WHITE) {
                search<WHITE>(engine, board, params, stats);
            } else {
                search<BLACK>(engine, board, params, stats);
            }

            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;

            nodes += stats.nodesSearched + stats.qNodesSearched;
            totalMs += elapsed.count();
        }
    }

    if (nodes == 0 || totalMs == 0) {
        engine.sendMessage("0 nodes 0 nps");
        return;
    }

    const double secondsSpent = totalMs / 1000.0;
    const uint64_t nodesPerSecond = static_cast<uint64_t>(static_cast<double>(nodes) / secondsSpent);
    std::string message = std::to_string(nodes) + " nodes " + std::to_string(nodesPerSecond) + " nps";

    engine.sendMessage(message);
}
