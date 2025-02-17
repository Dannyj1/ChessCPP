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

#include "eval.h"

#include <iostream>

#include "bitboard.h"
#include "bitwise.h"
#include "eval_features.h"
#include "types.h"

namespace Zagreus {
/**
 * \brief Adds the given midgame and endgame score to the given color.
 * \tparam color The color to add the score to.
 * \param midgameScore The midgame score to add.
 * \param endgameScore The endgame score to add.
 */
template <PieceColor color>
void Evaluation::addScore(const int midgameScore, const int endgameScore) {
    if (color == WHITE) {
        whiteMidgameScore += midgameScore;
        whiteEndgameScore += endgameScore;
    } else {
        blackMidgameScore += midgameScore;
        blackEndgameScore += endgameScore;
    }
}

/**
 * \brief Evaluates the current board position.
 *
 * This function calculates the evaluation score of the current board position
 * based on material and other features. It considers the phase of the game
 * (midgame or endgame) and adjusts the scores accordingly (tapered eval).
 *
 * \return The evaluation score of the current board position.
 */
int Evaluation::evaluate() {
    const int phase = calculatePhase();
    const int modifier = board.getSideToMove() == WHITE ? 1 : -1;

    initializeEvalData();

    evaluatePieces();

    const int whiteScore = ((whiteMidgameScore * (256 - phase)) + (whiteEndgameScore * phase)) / 256;
    const int blackScore = ((blackMidgameScore * (256 - phase)) + (blackEndgameScore * phase)) / 256;

    return (whiteScore - blackScore) * modifier;
}

constexpr int knightPhase = 1;
constexpr int bishopPhase = 1;
constexpr int rookPhase = 2;
constexpr int queenPhase = 4;
constexpr int totalPhase = knightPhase * 4 + bishopPhase * 4 + rookPhase * 4 + queenPhase * 2;

/**
 * \brief Calculates the phase of the game.
 *
 * This function calculates the phase of the game (midgame or endgame) based on
 * the remaining pieces on the board. The phase is used to adjust the evaluation
 * scores accordingly.
 *
 * \return The phase of the game as an integer.
 */
int Evaluation::calculatePhase() const {
    int phase = totalPhase;

    phase -= popcnt(board.getPieceBoard<WHITE_KNIGHT>() | board.getPieceBoard<BLACK_KNIGHT>()) * knightPhase;
    phase -= popcnt(board.getPieceBoard<WHITE_BISHOP>() | board.getPieceBoard<BLACK_BISHOP>()) * bishopPhase;
    phase -= popcnt(board.getPieceBoard<WHITE_ROOK>() | board.getPieceBoard<BLACK_ROOK>()) * rookPhase;
    phase -= popcnt(board.getPieceBoard<WHITE_QUEEN>() | board.getPieceBoard<BLACK_QUEEN>()) * queenPhase;

    return (phase * 256 + (totalPhase / 2)) / totalPhase;
}

void Evaluation::evaluatePieces() {
    evaluatePawns<WHITE>();
    evaluatePawns<BLACK>();

    evaluateKnights<WHITE>();
    evaluateKnights<BLACK>();

    evaluateBishops<WHITE>();
    evaluateBishops<BLACK>();

    evaluateRooks<WHITE>();
    evaluateRooks<BLACK>();

    evaluateQueens<WHITE>();
    evaluateQueens<BLACK>();

    evaluateKing<WHITE>();
    evaluateKing<BLACK>();
}

template <PieceColor color>
void Evaluation::evaluatePawns() {
    constexpr Piece pawnPiece = color == WHITE ? WHITE_PAWN : BLACK_PAWN;
    uint64_t pawns = board.getPieceBoard<pawnPiece>();

    while (pawns) {
        const Square square = static_cast<Square>(popLsb(pawns));
        const int midgamePst = midgamePstTable[pawnPiece][square];
        const int endgamePst = endgamePstTable[pawnPiece][square];

#ifdef ZAGREUS_TUNER
        trace.material[color][PAWN] += 1;
        trace.pst[color][PAWN][square] += 1;
#endif

        addScore<color>(midgamePst, endgamePst);

        const uint64_t attacks = getPawnAttacks<color>(square);

        evalData.attacksFrom[square] = attacks;
        evalData.attacksByColor[color] |= attacks;
        evalData.attacksByPiece[pawnPiece] |= attacks;
    }
}

/**
 * \brief Evaluates features related to knights on the board.
 */
template <PieceColor color>
void Evaluation::evaluateKnights() {
    constexpr Piece knightPiece = color == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT;
    uint64_t knights = board.getPieceBoard<knightPiece>();

    while (knights) {
        const Square square = static_cast<Square>(popLsb(knights));
        const int midgamePst = midgamePstTable[knightPiece][square];
        const int endgamePst = endgamePstTable[knightPiece][square];

#ifdef ZAGREUS_TUNER
        trace.material[color][KNIGHT] += 1;
        trace.pst[color][KNIGHT][square] += 1;
#endif

        addScore<color>(midgamePst, endgamePst);

        const uint64_t attacks = getKnightAttacks(square);

        evalData.attacksFrom[square] = attacks;
        evalData.attacksByColor[color] |= attacks;
        evalData.attacksByPiece[knightPiece] |= attacks;

        const uint64_t mobility = attacks & evalData.mobilityArea[color];
        const int mobilityScore = popcnt(mobility);
        const int midgameMobilityScore = evalMobility[MIDGAME][KNIGHT] * mobilityScore;
        const int endgameMobilityScore = evalMobility[ENDGAME][KNIGHT] * mobilityScore;

#ifdef ZAGREUS_TUNER
        trace.mobility[color][PAWN] += mobilityScore;
#endif

        addScore<color>(midgameMobilityScore, endgameMobilityScore);
    }
}

template <PieceColor color>
void Evaluation::evaluateBishops() {
    constexpr Piece bishopPiece = color == WHITE ? WHITE_BISHOP : BLACK_BISHOP;
    uint64_t bishops = board.getPieceBoard<bishopPiece>();

    while (bishops) {
        const Square square = static_cast<Square>(popLsb(bishops));
        const int midgamePst = midgamePstTable[bishopPiece][square];
        const int endgamePst = endgamePstTable[bishopPiece][square];

#ifdef ZAGREUS_TUNER
        trace.material[color][BISHOP] += 1;
        trace.pst[color][BISHOP][square] += 1;
#endif

        addScore<color>(midgamePst, endgamePst);

        const uint64_t attacks = getBishopAttacks(square, board.getOccupiedBitboard());

        evalData.attacksFrom[square] = attacks;
        evalData.attacksByColor[color] |= attacks;
        evalData.attacksByPiece[bishopPiece] |= attacks;

        const uint64_t mobility = attacks & evalData.mobilityArea[color];
        const int mobilityScore = popcnt(mobility);
        const int midgameMobilityScore = evalMobility[MIDGAME][BISHOP] * mobilityScore;
        const int endgameMobilityScore = evalMobility[ENDGAME][BISHOP] * mobilityScore;

#ifdef ZAGREUS_TUNER
        trace.mobility[color][BISHOP] += mobilityScore;
#endif

        addScore<color>(midgameMobilityScore, endgameMobilityScore);
    }
}

template <PieceColor color>
void Evaluation::evaluateRooks() {
    constexpr Piece rookPiece = color == WHITE ? WHITE_ROOK : BLACK_ROOK;
    uint64_t rooks = board.getPieceBoard<rookPiece>();

    while (rooks) {
        const Square square = static_cast<Square>(popLsb(rooks));
        const int midgamePst = midgamePstTable[rookPiece][square];
        const int endgamePst = endgamePstTable[rookPiece][square];

#ifdef ZAGREUS_TUNER
        trace.material[color][ROOK] += 1;
        trace.pst[color][ROOK][square] += 1;
#endif

        addScore<color>(midgamePst, endgamePst);

        const uint64_t attacks = getRookAttacks(square, board.getOccupiedBitboard());

        evalData.attacksFrom[square] = attacks;
        evalData.attacksByColor[color] |= attacks;
        evalData.attacksByPiece[rookPiece] |= attacks;

        const uint64_t mobility = attacks & evalData.mobilityArea[color];
        const int mobilityScore = popcnt(mobility);
        const int midgameMobilityScore = evalMobility[MIDGAME][ROOK] * mobilityScore;
        const int endgameMobilityScore = evalMobility[ENDGAME][ROOK] * mobilityScore;

#ifdef ZAGREUS_TUNER
        trace.mobility[color][ROOK] += mobilityScore;
#endif

        addScore<color>(midgameMobilityScore, endgameMobilityScore);
    }
}

template <PieceColor color>
void Evaluation::evaluateQueens() {
    constexpr Piece queenPiece = color == WHITE ? WHITE_QUEEN : BLACK_QUEEN;
    uint64_t queens = board.getPieceBoard<queenPiece>();

    while (queens) {
        const Square square = static_cast<Square>(popLsb(queens));
        const int midgamePst = midgamePstTable[queenPiece][square];
        const int endgamePst = endgamePstTable[queenPiece][square];

#ifdef ZAGREUS_TUNER
        trace.material[color][QUEEN] += 1;
        trace.pst[color][QUEEN][square] += 1;
#endif

        addScore<color>(midgamePst, endgamePst);

        const uint64_t attacks = queenAttacks(square, board.getOccupiedBitboard());

        evalData.attacksFrom[square] = attacks;
        evalData.attacksByColor[color] |= attacks;
        evalData.attacksByPiece[queenPiece] |= attacks;

        const uint64_t mobility = attacks & evalData.mobilityArea[color];
        const int mobilityScore = popcnt(mobility);
        const int midgameMobilityScore = evalMobility[MIDGAME][QUEEN] * mobilityScore;
        const int endgameMobilityScore = evalMobility[ENDGAME][QUEEN] * mobilityScore;

#ifdef ZAGREUS_TUNER
        trace.mobility[color][QUEEN] += mobilityScore;
#endif

        addScore<color>(midgameMobilityScore, endgameMobilityScore);
    }
}

template <PieceColor color>
void Evaluation::evaluateKing() {
    constexpr Piece kingPiece = color == WHITE ? WHITE_KING : BLACK_KING;
    const Square square = board.getKingSquare<color>();

    const int midgamePst = midgamePstTable[kingPiece][square];
    const int endgamePst = endgamePstTable[kingPiece][square];

#ifdef ZAGREUS_TUNER
    trace.material[color][KING] += 1;
    trace.pst[color][KING][square] += 1;
#endif

    addScore<color>(midgamePst, endgamePst);

    const uint64_t attacks = getKingAttacks(square);

    evalData.attacksFrom[square] = attacks;
    evalData.attacksByColor[color] |= attacks;
    evalData.attacksByPiece[kingPiece] |= attacks;
}

/**
 * \brief Initializes part of the evaluation data needed to evaluate the board position.
 */
void Evaluation::initializeEvalData() {
    evalData.mobilityArea[WHITE] = ~(board.getColorBitboard<WHITE>());
    evalData.mobilityArea[BLACK] = ~(board.getColorBitboard<BLACK>());
}

/**
 * \brief Gets the value of a given piece.
 *
 * @param piece The piece to get the value of.
 * @return The value of the given piece.
 */
int getPieceValue(const Piece piece) {
    switch (piece) {
        case WHITE_PAWN:
        case BLACK_PAWN:
            return 100;
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            return 350;
        case WHITE_BISHOP:
        case BLACK_BISHOP:
            return 350;
        case WHITE_ROOK:
        case BLACK_ROOK:
            return 525;
        case WHITE_QUEEN:
        case BLACK_QUEEN:
            return 1000;
        case WHITE_KING:
        case BLACK_KING:
            return 0;
        default:
            assert(false);
            return 0;
    }
}
} // namespace Zagreus
