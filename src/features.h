/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2024  Danny Jelsma

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
    MIDGAME_KING_ATTACK_PAWN_PENALTY,
    ENDGAME_KING_ATTACK_PAWN_PENALTY,
    MIDGAME_KING_ATTACK_KNIGHT_PENALTY,
    ENDGAME_KING_ATTACK_KNIGHT_PENALTY,
    MIDGAME_KING_ATTACK_BISHOP_PENALTY,
    ENDGAME_KING_ATTACK_BISHOP_PENALTY,
    MIDGAME_KING_ATTACK_ROOK_PENALTY,
    ENDGAME_KING_ATTACK_ROOK_PENALTY,
    MIDGAME_KING_ATTACK_QUEEN_PENALTY,
    ENDGAME_KING_ATTACK_QUEEN_PENALTY,
    MIDGAME_DOUBLED_PAWN_PENALTY,
    ENDGAME_DOUBLED_PAWN_PENALTY,
    MIDGAME_PASSED_PAWN,
    ENDGAME_PASSED_PAWN,
    MIDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY,
    ENDGAME_ISOLATED_SEMI_OPEN_PAWN_PENALTY,
    MIDGAME_ISOLATED_PAWN_PENALTY,
    ENDGAME_ISOLATED_PAWN_PENALTY,
    MIDGAME_ISOLATED_CENTRAL_PAWN_PENALTY,
    ENDGAME_ISOLATED_CENTRAL_PAWN_PENALTY,
    MIDGAME_KNIGHT_MISSING_PAWN_PENALTY,
    ENDGAME_KNIGHT_MISSING_PAWN_PENALTY,
    MIDGAME_KNIGHT_DEFENDED_BY_PAWN,
    ENDGAME_KNIGHT_DEFENDED_BY_PAWN,
    MIDGAME_MINOR_PIECE_NOT_DEFENDED_PENALTY,
    ENDGAME_MINOR_PIECE_NOT_DEFENDED_PENALTY,
    MIDGAME_BAD_BISHOP_PENALTY,
    ENDGAME_BAD_BISHOP_PENALTY,
    MIDGAME_MISSING_BISHOP_PAIR_PENALTY,
    ENDGAME_MISSING_BISHOP_PAIR_PENALTY,
    MIDGAME_BISHOP_FIANCHETTO,
    ENDGAME_BISHOP_FIANCHETTO,
    MIDGAME_ROOK_PAWN_COUNT,
    ENDGAME_ROOK_PAWN_COUNT,
    MIDGAME_ROOK_ON_OPEN_FILE,
    ENDGAME_ROOK_ON_OPEN_FILE,
    MIDGAME_ROOK_ON_SEMI_OPEN_FILE,
    ENDGAME_ROOK_ON_SEMI_OPEN_FILE,
    MIDGAME_ROOK_ON_7TH_RANK,
    ENDGAME_ROOK_ON_7TH_RANK,
    MIDGAME_TARRASCH_OWN_ROOK_PENALTY,
    ENDGAME_TARRASCH_OWN_ROOK_PENALTY,
    MIDGAME_TARRASCH_OWN_ROOK_DEFEND,
    ENDGAME_TARRASCH_OWN_ROOK_DEFEND,
    MIDGAME_TARRASCH_OPPONENT_ROOK_PENALTY,
    ENDGAME_TARRASCH_OPPONENT_ROOK_PENALTY,
    MIDGAME_ROOK_ON_QUEEN_FILE,
    ENDGAME_ROOK_ON_QUEEN_FILE,
    MIDGAME_MINOR_PIECE_ON_WEAK_SQUARE_PENALTY,
    ENDGAME_MINOR_PIECE_ON_WEAK_SQUARE_PENALTY,
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
    "MIDGAME_KING_ATTACK_PAWN_PENALTY",
    "ENDGAME_KING_ATTACK_PAWN_PENALTY",
    "MIDGAME_KING_ATTACK_KNIGHT_PENALTY",
    "ENDGAME_KING_ATTACK_KNIGHT_PENALTY",
    "MIDGAME_KING_ATTACK_BISHOP_PENALTY",
    "ENDGAME_KING_ATTACK_BISHOP_PENALTY",
    "MIDGAME_KING_ATTACK_ROOK_PENALTY",
    "ENDGAME_KING_ATTACK_ROOK_PENALTY",
    "MIDGAME_KING_ATTACK_QUEEN_PENALTY",
    "ENDGAME_KING_ATTACK_QUEEN_PENALTY",
    "MIDGAME_DOUBLED_PAWN_PENALTY",
    "ENDGAME_DOUBLED_PAWN_PENALTY",
    "MIDGAME_PASSED_PAWN",
    "ENDGAME_PASSED_PAWN",
    "MIDGAME_ISOLATED_SEMI_OPEN_PAWN",
    "ENDGAME_ISOLATED_SEMI_OPEN_PAWN",
    "MIDGAME_ISOLATED_PAWN",
    "ENDGAME_ISOLATED_PAWN",
    "MIDGAME_ISOLATED_CENTRAL_PAWN_PENALTY",
    "ENDGAME_ISOLATED_CENTRAL_PAWN_PENALTY",
    "MIDGAME_KNIGHT_MISSING_PAWN_PENALTY",
    "ENDGAME_KNIGHT_MISSING_PAWN_PENALTY",
    "MIDGAME_KNIGHT_DEFENDED_BY_PAWN",
    "ENDGAME_KNIGHT_DEFENDED_BY_PAWN",
    "MIDGAME_MINOR_PIECE_NOT_DEFENDED_PENALTY",
    "ENDGAME_MINOR_PIECE_NOT_DEFENDED_PENALTY",
    "MIDGAME_BAD_BISHOP_PENALTY",
    "ENDGAME_BAD_BISHOP_PENALTY",
    "MIDGAME_MISSING_BISHOP_PAIR_PENALTY",
    "ENDGAME_MISSING_BISHOP_PAIR_PENALTY",
    "MIDGAME_BISHOP_FIANCHETTO",
    "ENDGAME_BISHOP_FIANCHETTO",
    "MIDGAME_ROOK_PAWN_COUNT",
    "ENDGAME_ROOK_PAWN_COUNT",
    "MIDGAME_ROOK_ON_OPEN_FILE",
    "ENDGAME_ROOK_ON_OPEN_FILE",
    "MIDGAME_ROOK_ON_SEMI_OPEN_FILE",
    "ENDGAME_ROOK_ON_SEMI_OPEN_FILE",
    "MIDGAME_ROOK_ON_7TH_RANK",
    "MIDGAME_ROOK_ON_7TH_RANK",
    "MIDGAME_TARRASCH_OWN_ROOK_PENALTY",
    "ENDGAME_TARRASCH_OWN_ROOK_PENALTY",
    "MIDGAME_TARRASCH_OWN_ROOK_DEFEND",
    "ENDGAME_TARRASCH_OWN_ROOK_DEFEND",
    "MIDGAME_TARRASCH_OPPONENT_ROOK_PENALTY",
    "ENDGAME_TARRASCH_OPPONENT_ROOK_PENALTY",
    "MIDGAME_ROOK_ON_QUEEN_FILE",
    "ENDGAME_ROOK_ON_QUEEN_FILE",
    "MIDGAME_MINOR_PIECE_ON_WEAK_SQUARE_PENALTY",
    "ENDGAME_MINOR_PIECE_ON_WEAK_SQUARE_PENALTY",
};

void printEvalValues();

int getEvalValue(EvalFeature feature);

int getEvalFeatureSize();

std::vector<float> getEvalValues();

std::vector<float> getBaseEvalValues();

void updateEvalValues(std::vector<float>& newValues);
} // namespace Zagreus