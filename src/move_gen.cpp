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

#include "move_gen.h"
#include <cassert>
#include <array>
#include <initializer_list>
#include "bitboard.h"
#include "bitwise.h"
#include "board.h"
#include "constants.h"
#include "move.h"
#include "types.h"

namespace Zagreus {

/**
 * \brief Generates all pseudo-legal moves for all pieces of a certain color for a given color and generation type.
 * \tparam color The color of the pieces to generate moves for.
 * \tparam type The type of moves to generate (e.g., all moves, captures, quiet moves).
 * \param board The board object for which to generate moves.
 * \param[out] moves The list to store the generated moves.
 */
template <PieceColor color, GenerationType type>
void generateMoves(const Board& board, MoveList& moves) {
    assert(moves.size == 0);

    constexpr Piece ownKing = color == WHITE ? WHITE_KING : BLACK_KING;
    constexpr Piece opponentKing = color == WHITE ? BLACK_KING : WHITE_KING;
    constexpr PieceColor opponentColor = !color;
    bool onlyKingMoves = false;
    const uint64_t ownPieces = board.getColorBitboard<color>();
    const uint64_t opponentKingBB = board.getPieceBoard<opponentKing>();
    uint64_t genMask = ~(ownPieces | opponentKingBB);

    if (type == QSEARCH) {
        const uint64_t opponentPieces = board.getColorBitboard<opponentColor>();

        genMask &= opponentPieces;
    } else if (type == EVASIONS) {
        const Square kingSquare = bitboardToSquare(board.getPieceBoard<ownKing>());
        const uint64_t attackers = board.getSquareAttackersByColor<opponentColor>(kingSquare);

        if (popcnt(attackers) == 1) {
            const Square attackerSquare = bitboardToSquare(attackers);
            const PieceType attackerType = getPieceType(board.getPieceOnSquare(attackerSquare));
            uint64_t evasionsMask = attackers;

            if (isSlidingPiece(attackerType)) {
                const uint64_t squaresBetween = getSquaresBetween(attackerSquare, kingSquare);

                evasionsMask |= squaresBetween;
            }

            genMask = evasionsMask;
        } else {
            // If the king is in double check, only king moves are legal
            onlyKingMoves = true;
        }
    }

    if (!onlyKingMoves) {
        generatePawnMoves<color, type>(board, moves, genMask);
        generateKnightMoves<color, type>(board, moves, genMask);
        generateBishopMoves<color, type>(board, moves, genMask);
        generateRookMoves<color, type>(board, moves, genMask);
        generateQueenMoves<color, type>(board, moves, genMask);
    }

    if (type == EVASIONS) {
        // We generate all king moves when in check, so we don't use genMask
        generateKingMoves<color, type>(board, moves, ~(ownPieces | opponentKingBB));
    } else {
        generateKingMoves<color, type>(board, moves, genMask);
    }

    assert(type == QSEARCH || genMask != 0);
    assert((genMask & ownPieces) == 0);
    assert((genMask & opponentKingBB) == 0);
}

/**
 * \brief Generates all pseudo-legal pawn moves for a given color and generation type.
 * \tparam color The color of the pawns to generate moves for.
 * \tparam type The type of moves to generate (e.g., all moves, captures, quiet moves).
 * \param board The board object for which to generate moves.
 * \param[out] moves The list to store the generated moves.
 * \param genMask The mask to filter out invalid moves.
 */
template <PieceColor color, GenerationType type>
void generatePawnMoves(const Board& board, MoveList& moves, const uint64_t genMask) {
    constexpr Piece pawn = color == WHITE ? WHITE_PAWN : BLACK_PAWN;
    constexpr PieceColor opponentColor = !color;

    const uint64_t pawnBB = board.getPieceBoard<pawn>();
    const uint64_t emptyBB = board.getEmptyBitboard();
    const uint64_t opponentPieces = board.getColorBitboard<opponentColor>();
    uint64_t pawnSinglePushes;
    uint64_t pawnDoublePushes;
    uint64_t pawnWestAttacks;
    uint64_t pawnEastAttacks;

    if constexpr (color == WHITE) {
        pawnSinglePushes = whitePawnSinglePush(pawnBB, emptyBB);
        pawnDoublePushes = whitePawnDoublePush(pawnBB, emptyBB);
        pawnWestAttacks = whitePawnWestAttacks(pawnBB);
        pawnEastAttacks = whitePawnEastAttacks(pawnBB);
    } else {
        pawnSinglePushes = blackPawnSinglePush(pawnBB, emptyBB);
        pawnDoublePushes = blackPawnDoublePush(pawnBB, emptyBB);
        pawnWestAttacks = blackPawnWestAttacks(pawnBB);
        pawnEastAttacks = blackPawnEastAttacks(pawnBB);
    }

    uint64_t enPassantMask = squareToBitboard(board.getEnPassantSquare());

    if constexpr (color == WHITE) {
        enPassantMask &= RANK_6;
    } else {
        enPassantMask &= RANK_3;
    }

    pawnSinglePushes &= genMask;
    pawnDoublePushes &= genMask;
    pawnWestAttacks &= (opponentPieces | enPassantMask) & genMask;
    pawnEastAttacks &= (opponentPieces | enPassantMask) & genMask;

    constexpr Direction fromPushDirection = color == WHITE ? NORTH : SOUTH;
    constexpr Direction fromSqWestAttackDirection = color == WHITE ? NORTH_WEST : SOUTH_WEST;
    constexpr Direction fromSqEastAttackDirection = color == WHITE ? NORTH_EAST : SOUTH_EAST;
    constexpr uint64_t promotionRank = color == WHITE ? RANK_8 : RANK_1;

    while (pawnSinglePushes) {
        const uint8_t squareTo = popLsb(pawnSinglePushes);
        const uint64_t squareToBB = squareToBitboard(squareTo);
        const uint8_t squareFrom = squareTo - fromPushDirection;

        if (squareToBB & promotionRank) {
            for (const PromotionPiece promotionPiece : {QUEEN_PROMOTION, ROOK_PROMOTION, BISHOP_PROMOTION,
                                                        KNIGHT_PROMOTION}) {
                const Move move = encodeMove(squareFrom, squareTo, promotionPiece);
                moves.moves[moves.size] = move;
                moves.size++;
            }
        } else {
            const Move move = encodeMove(squareFrom, squareTo);

            moves.moves[moves.size] = move;
            moves.size++;
        }
    }

    while (pawnDoublePushes) {
        const uint8_t squareTo = popLsb(pawnDoublePushes);
        const uint8_t squareFrom = squareTo - fromPushDirection - fromPushDirection;
        const Move move = encodeMove(squareFrom, squareTo);

        moves.moves[moves.size] = move;
        moves.size++;
    }

    while (pawnWestAttacks) {
        const uint8_t squareTo = popLsb(pawnWestAttacks);
        const uint64_t squareToBB = squareToBitboard(squareTo);
        const uint8_t squareFrom = squareTo - fromSqWestAttackDirection;

        if (squareTo == board.getEnPassantSquare() && squareToBB & enPassantMask) {
            const Move move = encodeMove(squareFrom, squareTo, EN_PASSANT);
            moves.moves[moves.size] = move;
            moves.size++;
        } else {
            if (squareToBB & promotionRank) {
                for (const PromotionPiece promotionPiece : {QUEEN_PROMOTION, ROOK_PROMOTION, BISHOP_PROMOTION,
                                                            KNIGHT_PROMOTION}) {
                    const Move move = encodeMove(squareFrom, squareTo, promotionPiece);
                    moves.moves[moves.size] = move;
                    moves.size++;
                }
            } else {
                const Move move = encodeMove(squareFrom, squareTo);
                moves.moves[moves.size] = move;
                moves.size++;
            }
        }
    }

    while (pawnEastAttacks) {
        const uint8_t squareTo = popLsb(pawnEastAttacks);
        const uint64_t squareToBB = squareToBitboard(squareTo);
        const uint8_t squareFrom = squareTo - fromSqEastAttackDirection;

        if (squareTo == board.getEnPassantSquare() && squareToBB & enPassantMask) {
            const Move move = encodeMove(squareFrom, squareTo, EN_PASSANT);
            moves.moves[moves.size] = move;
            moves.size++;
        } else {
            if (squareToBB & promotionRank) {
                for (const PromotionPiece promotionPiece : {QUEEN_PROMOTION, ROOK_PROMOTION, BISHOP_PROMOTION,
                                                            KNIGHT_PROMOTION}) {
                    const Move move = encodeMove(squareFrom, squareTo, promotionPiece);
                    moves.moves[moves.size] = move;
                    moves.size++;
                }
            } else {
                const Move move = encodeMove(squareFrom, squareTo);
                moves.moves[moves.size] = move;
                moves.size++;
            }
        }
    }
}

/**
 * \brief Generates all pseudo-legal knight moves for a given color and generation type.
 * \tparam color The color of the knights to generate moves for.
 * \tparam type The type of moves to generate (e.g., all moves, captures, quiet moves).
 * \param board The board object for which to generate moves.
 * \param[out] moves The list to store the generated moves.
 * \param genMask The mask to filter out invalid moves.
 */
template <PieceColor color, GenerationType type>
void generateKnightMoves(const Board& board, MoveList& moves, const uint64_t genMask) {
    constexpr Piece knight = color == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT;
    uint64_t knightBB = board.getPieceBoard<knight>();

    while (knightBB) {
        const uint8_t fromSquare = popLsb(knightBB);
        const uint64_t attacks = getKnightAttacks(fromSquare);
        uint64_t genBB = attacks & genMask;

        while (genBB) {
            const uint8_t toSquare = popLsb(genBB);
            const Move move = encodeMove(fromSquare, toSquare);

            moves.moves[moves.size] = move;
            moves.size++;
        }
    }
}

/**
 * \brief Generates all pseudo-legal bishop moves for a given color and generation type.
 * \tparam color The color of the bishops to generate moves for.
 * \tparam type The type of moves to generate (e.g., all moves, captures, quiet moves).
 * \param board The board object for which to generate moves.
 * \param[out] moves The list to store the generated moves.
 * \param genMask The mask to filter out invalid moves.
 */
template <PieceColor color, GenerationType type>
void generateBishopMoves(const Board& board, MoveList& moves, const uint64_t genMask) {
    constexpr Piece bishop = color == WHITE ? WHITE_BISHOP : BLACK_BISHOP;
    const uint64_t occupied = board.getOccupiedBitboard();
    uint64_t bishopBB = board.getPieceBoard<bishop>();

    while (bishopBB) {
        const uint8_t fromSquare = popLsb(bishopBB);
        uint64_t genBB = getBishopAttacks(fromSquare, occupied) & genMask;

        while (genBB) {
            const uint8_t toSquare = popLsb(genBB);
            const Move move = encodeMove(fromSquare, toSquare);

            moves.moves[moves.size] = move;
            moves.size++;
        }
    }
}

/**
 * \brief Generates all pseudo-legal rook moves for a given color and generation type.
 * \tparam color The color of the rooks to generate moves for.
 * \tparam type The type of moves to generate (e.g., all moves, captures, quiet moves).
 * \param board The board object for which to generate moves.
 * \param[out] moves The list to store the generated moves.
 * \param genMask The mask to filter out invalid moves.
 */
template <PieceColor color, GenerationType type>
void generateRookMoves(const Board& board, MoveList& moves, const uint64_t genMask) {
    constexpr Piece rook = color == WHITE ? WHITE_ROOK : BLACK_ROOK;
    const uint64_t occupied = board.getOccupiedBitboard();
    uint64_t rookBB = board.getPieceBoard<rook>();

    while (rookBB) {
        const uint8_t fromSquare = popLsb(rookBB);
        uint64_t genBB = getRookAttacks(fromSquare, occupied) & genMask;

        while (genBB) {
            const uint8_t toSquare = popLsb(genBB);
            const Move move = encodeMove(fromSquare, toSquare);

            moves.moves[moves.size] = move;
            moves.size++;
        }
    }
}

/**
 * \brief Generates all pseudo-legal queen moves for a given color and generation type.
 * \tparam color The color of the queens to generate moves for.
 * \tparam type The type of moves to generate (e.g., all moves, captures, quiet moves).
 * \param board The board object for which to generate moves.
 * \param[out] moves The list to store the generated moves.
 * \param genMask The mask to filter out invalid moves.
 */
template <PieceColor color, GenerationType type>
void generateQueenMoves(const Board& board, MoveList& moves, const uint64_t genMask) {
    constexpr Piece queen = color == WHITE ? WHITE_QUEEN : BLACK_QUEEN;
    const uint64_t occupied = board.getOccupiedBitboard();
    uint64_t queenBB = board.getPieceBoard<queen>();

    while (queenBB) {
        const uint8_t fromSquare = popLsb(queenBB);
        uint64_t genBB = queenAttacks(fromSquare, occupied) & genMask;

        while (genBB) {
            const uint8_t toSquare = popLsb(genBB);
            const Move move = encodeMove(fromSquare, toSquare);

            moves.moves[moves.size] = move;
            moves.size++;
        }
    }
}

/**
 * \brief Generates all pseudo-legal king moves for a given color and generation type.
 * \tparam color The color of the king to generate moves for.
 * \tparam type The type of moves to generate (e.g., all moves, captures, quiet moves).
 * \param board The board object for which to generate moves.
 * \param[out] moves The list to store the generated moves.
 * \param genMask The mask to filter out invalid moves.
 */
template <PieceColor color, GenerationType type>
void generateKingMoves(const Board& board, MoveList& moves, const uint64_t genMask) {
    constexpr Piece king = color == WHITE ? WHITE_KING : BLACK_KING;
    uint64_t kingBB = board.getPieceBoard<king>();
    const uint8_t fromSquare = popLsb(kingBB);
    uint64_t genBB = getKingAttacks(fromSquare) & genMask;

    while (genBB) {
        const uint8_t toSquare = popLsb(genBB);
        const Move move = encodeMove(fromSquare, toSquare);

        moves.moves[moves.size] = move;
        moves.size++;
    }

    const uint8_t castlingRights = board.getCastlingRights();

    if constexpr (color == WHITE) {
        if (castlingRights & WHITE_KINGSIDE && board.canCastle<WHITE_KINGSIDE>()) {
            const Move move = encodeMove(E1, G1, CASTLING);

            moves.moves[moves.size] = move;
            moves.size++;
        }

        if (castlingRights & WHITE_QUEENSIDE && board.canCastle<WHITE_QUEENSIDE>()) {
            const Move move = encodeMove(E1, C1, CASTLING);

            moves.moves[moves.size] = move;
            moves.size++;
        }
    } else if constexpr (color == BLACK) {
        if (castlingRights & BLACK_KINGSIDE && board.canCastle<BLACK_KINGSIDE>()) {
            const Move move = encodeMove(E8, G8, CASTLING);

            moves.moves[moves.size] = move;
            moves.size++;
        }

        if (castlingRights & BLACK_QUEENSIDE && board.canCastle<BLACK_QUEENSIDE>()) {
            const Move move = encodeMove(E8, C8, CASTLING);

            moves.moves[moves.size] = move;
            moves.size++;
        }
    }
}

// explicit instantiation of generateMoves
template void generateMoves<WHITE, ALL>(const Board& board, MoveList& moves);
template void generateMoves<WHITE, QSEARCH>(const Board& board, MoveList& moves);
template void generateMoves<WHITE, EVASIONS>(const Board& board, MoveList& moves);
template void generateMoves<BLACK, ALL>(const Board& board, MoveList& moves);
template void generateMoves<BLACK, QSEARCH>(const Board& board, MoveList& moves);
template void generateMoves<BLACK, EVASIONS>(const Board& board, MoveList& moves);

} // namespace Zagreus
