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

#include "../src/types.h"
#include "../src/constants.h"
#include "../src/move.h"

namespace Zagreus {
TEST_CASE("test_getMoveNotation", "[uci]") {
    const std::array<std::string, 64> squareStrings = {{
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
    }};

    for (uint8_t from = 0; from < squareStrings.size(); ++from) {
        for (uint8_t to = 0; to < squareStrings.size(); ++to) {
            if (from == to) {
                // These moves will never happen
                continue;
            }

            std::string expected = squareStrings[from] + squareStrings[to];

            CAPTURE(from, to, expected);
            REQUIRE(getMoveNotation(from, to) == expected);
        }
    }
}

TEST_CASE("test_getMoveNotation_Promotion", "[uci]") {
    const std::array<std::string, 64> squareStrings = {{
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
    }};

    auto promotionLetter = [](const PromotionPiece piece) -> std::string {
        switch(piece) {
            case QUEEN_PROMOTION:  return "q";
            case ROOK_PROMOTION:   return "r";
            case BISHOP_PROMOTION: return "b";
            case KNIGHT_PROMOTION: return "n";
            default:               return "";
        }
    };

    // Loop over every valid move (skip moves where the fromSquare equals the toSquare)
    // and then for each valid move, test all four promotion piece options.
    for (uint8_t from = 0; from < squareStrings.size(); ++from) {
        for (uint8_t to = 0; to < squareStrings.size(); ++to) {
            if (from == to)
                continue;  // Skip moves where there is no actual movement.

            // For each promotion piece.
            for (uint8_t p = 0; p < 4; ++p) {
                const PromotionPiece promo = static_cast<PromotionPiece>(p);
                // Construct the expected move notation using the lookup table for squares.
                std::string expected = squareStrings[from] + squareStrings[to] + promotionLetter(promo);
                CAPTURE(from, to, p, expected);
                REQUIRE(getMoveNotation(from, to, promo) == expected);
            }
        }
    }
}
} // namespace Zagreus
