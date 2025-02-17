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

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <string_view>
#include "bitboard.h"
#include "bitwise.h"
#include "constants.h"
#include "move.h"
#include "types.h"

namespace Zagreus {
/**
 * \brief Initializes the Zobrist constants.
 */
void initZobristConstants();

/**
 * \brief Gets the Zobrist constant for a given index.
 */
[[nodiscard]] constexpr uint64_t getZobristConstant(int index);

/**
 * \brief Represents the state of the board at a given ply
 */
struct BoardState {
    uint64_t zobristHash = 0;
    Move move = NO_MOVE;
    Move previousMove = NO_MOVE;
    Piece capturedPiece = EMPTY;
    uint8_t enPassantSquare = 0;
    uint8_t castlingRights = 0;
    uint8_t halfMoveClock = 0;
};

/**
 * \brief Represents the chess board and provides methods to manipulate it. The board consists of several bitboards.
 */
class Board {
private:
    std::array<Piece, SQUARES> board{};
    std::array<uint64_t, PIECES> bitboards{};
    std::array<uint64_t, COLORS> colorBoards{};
    std::array<BoardState, MAX_PLIES> history{};
    PvLine previousPvLine{0};
    PieceColor sideToMove = WHITE;
    uint64_t occupied = 0;
    uint64_t zobristHash = 0;
    Move previousMove = NO_MOVE;
    uint16_t ply = 0;
    uint16_t fullmoveClock = 1;
    uint8_t halfMoveClock = 0;
    uint8_t castlingRights = 0;
    uint8_t enPassantSquare = 255;

public:
    /**
     * \brief Constructs a new Board object and initializes it to the starting position.
     */
    Board() {
        std::ranges::fill(board, EMPTY);
        std::ranges::fill(bitboards, 0);
        std::ranges::fill(colorBoards, 0);
        std::ranges::fill(history, BoardState{});
    }

    Board(const Board&) = delete;

    Board& operator=(const Board&) = delete;

    /**
     * \brief Retrieves the bitboard for a given piece type.
     * \tparam piece The piece type.
     * \return The bitboard for the given piece type.
     */
    template <Piece piece>
    [[nodiscard]] constexpr uint64_t getPieceBoard() const {
        return bitboards[piece];
    }

    /**
     * \brief Retrieves the bitboard for a given piece type.
     * \tparam piece The piece type.
     * \return The bitboard for the given piece type.
     */
    [[nodiscard]] constexpr uint64_t getPieceBoard(const Piece piece) const {
        return bitboards[piece];
    }

    /**
     * \brief Retrieves the bitboard for a given color.
     * \tparam color The color.
     * \return The bitboard for the given color.
     */
    template <PieceColor color>
    [[nodiscard]] constexpr uint64_t getColorBitboard() const {
        return colorBoards[color];
    }

    /**
     * \brief Retrieves the bitboard for a given color.
     * \param color The color.
     * \return The bitboard for the given color.
     */
    [[nodiscard]] constexpr uint64_t getColorBitboard(PieceColor color) const {
        return colorBoards[color];
    }

    /**
     * \brief Retrieves the piece on a given square.
     * \param square The square index (0-63).
     * \return The piece on the given square.
     */
    [[nodiscard]] constexpr Piece getPieceOnSquare(const int square) const {
        assert(square >= 0 && square < 64);
        return board[square];
    }

    /**
     * \brief Checks if there is a piece on a given square.
     * \param square The square index (0-63).
     * \return True if there is a piece on the given square, false otherwise.
     */
    [[nodiscard]] constexpr bool isPieceOnSquare(const int square) const {
        return board[square] != EMPTY;
    }

    /**
     * \brief Retrieves the bitboard representing occupied squares.
     * \return The bitboard representing occupied squares.
     */
    [[nodiscard]] constexpr uint64_t getOccupiedBitboard() const {
        return occupied;
    }

    /**
     * \brief Retrieves the bitboard representing empty squares.
     * \return The bitboard representing empty squares.
     */
    [[nodiscard]] constexpr uint64_t getEmptyBitboard() const {
        return ~occupied;
    }

    /**
     * \brief Retrieves the color of the side to move.
     * \return The color of the side to move.
     */
    [[nodiscard]] constexpr PieceColor getSideToMove() const {
        return sideToMove;
    }

    /**
     * \brief Sets the side to move.
     */
    void setSideToMove(const PieceColor color) {
        sideToMove = color;
    }

    /**
     * \brief Retrieves the number of plies since the start of the game.
     * \return The number of plies since the start of the game.
     */
    [[nodiscard]] constexpr int getPly() const {
        return ply;
    }

    /**
     * \brief Retrieves the castling rights available.
     * \return The castling rights available.
     */
    [[nodiscard]] constexpr uint8_t getCastlingRights() const {
        return castlingRights;
    }

    /**
     * \brief Retrieves the en passant square, if applicable.
     * \return The en passant square.
     */
    [[nodiscard]] constexpr uint8_t getEnPassantSquare() const {
        return enPassantSquare;
    }

    /**
     * \brief Retrieves the last move made.
     * \return The last move made.
     */
    [[nodiscard]] constexpr Move getLastMove() const {
        return history[ply - 1].move;
    }

    /**
     * \brief sets the previous PV line.
     */
    void setPreviousPvLine(const PvLine& pvLine) {
        previousPvLine = pvLine;
    }

    /**
     * \brief Gets the previous PV line.
     *
     * \return The previous PV line.
     */
    PvLine& getPreviousPvLine() {
        return previousPvLine;
    }

    /**
     * \brief Sets a piece on a given square.
     * \tparam piece The piece type.
     * \param square The square index (0-63).
     */
    template <Piece piece>
    void setPiece(const uint8_t square) {
        assert(piece != Piece::EMPTY);
        assert(board[square] == Piece::EMPTY);
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = piece;
        bitboards[piece] |= squareBB;
        occupied |= squareBB;
        colorBoards[getPieceColor(piece)] |= squareBB;

        const int zobristIndex = ZOBRIST_PIECE_START_INDEX + static_cast<int>(piece) * SQUARES + square;
        zobristHash ^= getZobristConstant(zobristIndex);
    }

    /**
     * \brief Sets a piece on a given square.
     * \param piece The piece type.
     * \param square The square index (0-63).
     */
    void setPiece(const Piece piece, const uint8_t square) {
        assert(piece != Piece::EMPTY);
        assert(board[square] == Piece::EMPTY);
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = piece;
        bitboards[piece] |= squareBB;
        occupied |= squareBB;
        colorBoards[getPieceColor(piece)] |= squareBB;

        const int zobristIndex = ZOBRIST_PIECE_START_INDEX + static_cast<int>(piece) * SQUARES + square;
        zobristHash ^= getZobristConstant(zobristIndex);
    }

    /**
     * \brief Removes a piece from a given square.
     * \param square The square index (0-63).
     */
    void removePiece(const uint8_t square) {
        const uint64_t squareBB = squareToBitboard(square);
        const Piece piece = board[square];
        assert(piece != Piece::EMPTY);

        board[square] = EMPTY;
        bitboards[piece] &= ~squareBB;
        occupied &= ~squareBB;
        colorBoards[getPieceColor(piece)] &= ~squareBB;

        const int zobristIndex = ZOBRIST_PIECE_START_INDEX + static_cast<int>(piece) * SQUARES + square;
        zobristHash ^= getZobristConstant(zobristIndex);
    }

    /**
     * \brief Removes a piece from a given square.
     * \tparam piece The piece type.
     * \param square The square index (0-63).
     */
    template <Piece piece>
    void removePiece(const uint8_t square) {
        assert(piece != Piece::EMPTY);
        assert(board[square] == piece);
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = EMPTY;
        bitboards[piece] &= ~squareBB;
        occupied &= ~squareBB;
        colorBoards[getPieceColor(piece)] &= ~squareBB;

        const int zobristIndex = ZOBRIST_PIECE_START_INDEX + static_cast<int>(piece) * SQUARES + square;
        zobristHash ^= getZobristConstant(zobristIndex);
    }

    /**
     * \brief Removes a piece from a given square.
     * \param piece The piece type.
     * \param square The square index (0-63).
     */
    void removePiece(const Piece piece, const uint8_t square) {
        assert(piece != Piece::EMPTY);
        assert(board[square] == piece);
        const uint64_t squareBB = squareToBitboard(square);

        board[square] = EMPTY;
        bitboards[piece] &= ~squareBB;
        occupied &= ~squareBB;
        colorBoards[getPieceColor(piece)] &= ~squareBB;

        const int zobristIndex = ZOBRIST_PIECE_START_INDEX + static_cast<int>(piece) * SQUARES + square;
        zobristHash ^= getZobristConstant(zobristIndex);
    }

    /**
     * \brief Makes a move on the board.
     * \param move The move to make.
     */
    void makeMove(Move move);

    /**
     * \brief Unmakes the last move on the board.
     */
    void unmakeMove();

    /**
     * \brief Makes a null move on the board, meaning the side to move does not make a move and the turn is passed.
     */
    void makeNullMove();

    /**
     * \brief Unmakes the last null move on the board. Should only be called after a makeNullMove.
     */
    void unmakeNullMove();

    /**
     * \brief Sets a piece on the board from a FEN character.
     * \param character The FEN character representing the piece.
     * \param square The square index (0-63).
     */
    void setPieceFromFENChar(char character, uint8_t square);

    template <PieceColor color>
    [[nodiscard]] bool isKingInCheck() const {
        constexpr PieceColor opponentColor = !color;
        const uint64_t kingBB = getPieceBoard<color == WHITE ? WHITE_KING : BLACK_KING>();
        const Square kingSquare = bitboardToSquare(kingBB);

        return getSquareAttackersByColor<opponentColor>(kingSquare) != 0;
    }

    /**
     * \brief Checks if the current position is legal based on the color that just made a move.
     * \tparam movedColor The color of the player who just moved.
     * \return True if the position is legal, false otherwise.
     */
    template <PieceColor movedColor>
    [[nodiscard]] bool isPositionLegal() const;

    /**
     * \brief Retrieves the attackers of a given square.
     * \param square The square index (0-63).
     * \return A bitboard representing the attackers.
     */
    [[nodiscard]] uint64_t getSquareAttackers(Square square) const;

    /**
     * \brief Retrieves the attackers of a given square by color.
     * \tparam color The color of the attackers.
     * \param square The square index (0-63).
     * \return A bitboard representing the attackers of the given color.
     */
    template <PieceColor color>
    [[nodiscard]] uint64_t getSquareAttackersByColor(const Square square) const {
        return getSquareAttackers(square) & getColorBitboard<color>();
    }

    /**
     * \brief Checks if castling is possible for the given side.
     * \tparam side The side to check for castling (WHITE\_KINGSIDE, WHITE\_QUEENSIDE, BLACK\_KINGSIDE, BLACK\_QUEENSIDE).
     * \return True if castling is possible, false otherwise.
     */
    template <CastlingRights side>
    [[nodiscard]] bool canCastle() const;

    /**
     * \brief Sets the board state from a FEN string.
     * \param fen The FEN string representing the board state.
     * \return True if the FEN string was valid, false otherwise.
     */
    [[nodiscard]] bool setFromFEN(std::string_view fen);

    /**
     * \brief Resets the board to the initial state.
     */
    void reset();

    /**
     * \brief Prints the current state of the board to the console.
     */
    void print() const;

    /**
     * \brief Checks if the current position is a draw. Checks everything but a stalemate.
     *
     * \return True if the position is a draw, false otherwise.
     */
    bool isDraw() const;

    /**
     * \brief Gets the current zobrist hash of the board. It is pre-computed and will only be retrieved from memory.
     *
     * \return The current zobrist hash of the board.
     */
    uint64_t getZobristHash() const;

    /**
     * \brief gets the square of the attacker with the lowest value of a given square
     * \tparam color The color of the attacker.
     * \param square The square to evaluate.
     *
     * \return The square of the attacker with the lowest value.
     */
    template <PieceColor color>
    Square getSmallestAttacker(Square square) const;

    /**
     * \brief Calculates if the Static Exchange Evaluation (SEE) for a given move is good for the given color.
     * \param move The move to evaluate.
     *
     * \return True if the move is good for the given color, false otherwise.
     */
    bool see(Move move, int threshold);

    /**
     * \brief Retrieves the half move clock.
     * \return The half move clock.
     */
    [[nodiscard]] constexpr uint8_t getHalfMoveClock() const {
        return halfMoveClock;
    }

    /**
     * \brief Retrieves the full move clock.
     * \return The full move clock.
     */
    [[nodiscard]] constexpr uint16_t getFullMoveClock() const {
        return fullmoveClock;
    }

    /**
     * \brief Checks if the given color is close to promoting a pawn.
     * \return True if the given color is close to promoting a pawn, false otherwise.
     */
    template <PieceColor color>
    [[nodiscard]] bool canPromotePawn() const {
        const uint64_t promotionRank = color == WHITE ? RANK_7 : RANK_2;
        const uint64_t pawns = getPieceBoard<color == WHITE ? WHITE_PAWN : BLACK_PAWN>();

        return pawns & promotionRank;
    }

    /**
     * \brief Gets the previous move made.
     *
     * \return The previous move made.
     */
    [[nodiscard]] Move getPreviousMove() const {
        return previousMove;
    }

    /**
     * \brief Determines if the board has material that isn't pawn or king.
     *
     * \return True if the board has material that isn't pawn or king, false otherwise.
     */
    [[nodiscard]] bool hasNonPawnMaterial() const {
        return occupied & ~(getPieceBoard<WHITE_KING>() | getPieceBoard<BLACK_KING>() | getPieceBoard<WHITE_PAWN>() |
                            getPieceBoard<BLACK_PAWN>());
    }

    /**
     * \brief Determines if the board has material that isn't pawn or king for the given color.
     * \tparam color The color to check for material.
     *
     * \return True if the board has material that isn't pawn or king, false otherwise.
     */
    template <PieceColor color>
    [[nodiscard]] bool hasNonPawnMaterial() const {
        return getColorBitboard<color>() & ~(getPieceBoard<color == WHITE ? WHITE_KING : BLACK_KING>() | getPieceBoard<
                                                 color == WHITE ? WHITE_PAWN : BLACK_PAWN>());
    }

    template <PieceColor color>
    [[nodiscard]] Square getKingSquare() const {
        return bitboardToSquare(getPieceBoard<color == WHITE ? WHITE_KING : BLACK_KING>());
    }
};
} // namespace Zagreus
