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

#include <cstdint>
#include "board.h"
#include "constants.h"
#include "eval_features.h"

namespace Zagreus {

struct EvalData {
    uint64_t mobilityArea[COLORS];

    // These are initialized later by evaluatePieces
    uint64_t attacksFrom[SQUARES];
    uint64_t attacksByColor[COLORS];
    uint64_t attacksByPiece[PIECES];
};

#ifdef ZAGREUS_TUNER
struct EvalTrace {
    int material[COLORS][PIECE_TYPES]{};
    int pst[COLORS][PIECE_TYPES][SQUARES]{};
    int mobility[COLORS][PIECE_TYPES]{};
};
#endif

class Evaluation {
private:
    const Board& board;
    EvalData evalData{};
    int whiteMidgameScore{};
    int whiteEndgameScore{};
    int blackMidgameScore{};
    int blackEndgameScore{};

    /**
     * \brief Evaluates the material on the board.
     *
     * This function evaluates the material on the board for both sides (white and black)
     * and updates the midgame and endgame scores accordingly.
     */
    [[nodiscard]] int calculatePhase() const;

    /**
     * \brief Adds the given midgame and endgame score to the given color.
     * \tparam color The color to add the score to.
     * \param midgameScore The midgame score to add.
     * \param endgameScore The endgame score to add.
     */
    template <PieceColor color>
    void addScore(int midgameScore, int endgameScore);

    /**
     * \brief Evaluates several features related to pieces on the board.
     */
    void evaluatePieces();

    /**
    * \brief Evaluates features related to pawns on the board.
    */
    template <PieceColor color>
    void evaluatePawns();

    /**
     * \brief Evaluates features related to knights on the board.
     */
    template <PieceColor color>
    void evaluateKnights();

    /**
     * \brief Evaluates features related to bishops on the board.
     */
    template <PieceColor color>
    void evaluateBishops();

    /**
     * \brief Evaluates features related to rooks on the board.
     */
    template <PieceColor color>
    void evaluateRooks();

    /**
     * \brief Evaluates features related to queens on the board.
     */
    template <PieceColor color>
    void evaluateQueens();

    /**
     * \brief Evaluates features related to the king on the board.
     */
    template <PieceColor color>
    void evaluateKing();

public:
#ifdef ZAGREUS_TUNER
    EvalTrace trace{};
#endif

    /**
     * \brief Constructs an Evaluation object with the given board.
     * \param board The current state of the chess board.
     */
    explicit Evaluation(const Board& board) : board(board) {}

    Evaluation(const Evaluation&) = delete;

    Evaluation& operator=(const Evaluation&) = delete;


    /**
     * \brief Initializes part of the evaluation data needed to evaluate the board position.
     */
    void initializeEvalData();

    /**
     * \brief Evaluates the current board position.
     * \return The evaluation score of the current board position.
     */
    [[nodiscard]] int evaluate();
};

/**
 * \brief Gets the value of a given piece.
 *
 * @param piece The piece to get the value of.
 * @return The value of the given piece.
 */
[[nodiscard]] int getPieceValue(Piece piece);
} // namespace Zagreus
