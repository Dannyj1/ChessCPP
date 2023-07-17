/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023  Danny Jelsma

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

#include <vector>

namespace Zagreus {
    enum EvalFeature {
        MIDGAME_PAWN_MATERIAL,
        ENDGAME_PAWN_MATERIAL,
        MIDGAME_KNIGHT_MATERIAL,
        ENDGAME_KNIGHT_MATERIAL,
        MIDGAME_BISHOP_MATERIAL,
        ENDGAME_BISHOP_MATERIAL,
        MIDGAME_ROOK_MATERIAL,
        ENDGAME_ROOK_MATERIAL,
        MIDGAME_QUEEN_MATERIAL,
        ENDGAME_QUEEN_MATERIAL,
        MIDGAME_KNIGHT_MOBILITY,
        ENDGAME_KNIGHT_MOBILITY,
        MIDGAME_BISHOP_MOBILITY,
        ENDGAME_BISHOP_MOBILITY,
        MIDGAME_ROOK_MOBILITY,
        ENDGAME_ROOK_MOBILITY,
        MIDGAME_QUEEN_MOBILITY,
        ENDGAME_QUEEN_MOBILITY,
        MIDGAME_PAWN_SHIELD,
        ENDGAME_PAWN_SHIELD,
        MIDGAME_KING_VIRTUAL_MOBILITY_PENALTY,
        ENDGAME_KING_VIRTUAL_MOBILITY_PENALTY,
        MIDGAME_PAWN_STORM_PENALTY,
        ENDGAME_PAWN_STORM_PENALTY,
    };

    static std::vector<const char*> evalFeatureNames = {
            "MIDGAME_PAWN_MATERIAL",
            "ENDGAME_PAWN_MATERIAL",
            "MIDGAME_KNIGHT_MATERIAL",
            "ENDGAME_KNIGHT_MATERIAL",
            "MIDGAME_BISHOP_MATERIAL",
            "ENDGAME_BISHOP_MATERIAL",
            "MIDGAME_ROOK_MATERIAL",
            "ENDGAME_ROOK_MATERIAL",
            "MIDGAME_QUEEN_MATERIAL",
            "ENDGAME_QUEEN_MATERIAL",
            "MIDGAME_KNIGHT_MOBILITY",
            "ENDGAME_KNIGHT_MOBILITY",
            "MIDGAME_BISHOP_MOBILITY",
            "ENDGAME_BISHOP_MOBILITY",
            "MIDGAME_ROOK_MOBILITY",
            "ENDGAME_ROOK_MOBILITY",
            "MIDGAME_QUEEN_MOBILITY",
            "ENDGAME_QUEEN_MOBILITY",
            "MIDGAME_PAWN_SHIELD",
            "ENDGAME_PAWN_SHIELD",
            "MIDGAME_KING_VIRTUAL_MOBILITY_PENALTY",
            "ENDGAME_KING_VIRTUAL_MOBILITY_PENALTY",
            "MIDGAME_PAWN_STORM_PENALTY",
            "ENDGAME_PAWN_STORM_PENALTY",
    };

    void printEvalValues();

    int getEvalValue(EvalFeature feature);

    int getEvalFeatureSize();

    std::vector<double> getEvalValues();

    std::vector<double> getBaseEvalValues();

    void updateEvalValues(std::vector<double> &newValues);
}
