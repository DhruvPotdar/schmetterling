#pragma once

#include "board/bitboard.hpp"
#include "board/board.hpp"
#include "moves/moves.hpp"
#include <vector>

/**
 * @class MoveGenerator
 * @brief Takes in the board and generates all possible legal and pseudo legal moves for all pieces
 * on the board
 */
class MoveGenerator {

  public:
    MoveGenerator();
    MoveGenerator(const Board& board) : _board(board) {};

    const std::vector<Move> generateMoves();
    bool isLegalMove(const Move move) const;
    const std::vector<Move> generatePseudoLegalMoves();

    void generatePawnMoves(Square square, std::vector<Move>& moves);
    void generateKnightMoves(Square square, std::vector<Move>& moves);
    void generateBishopMoves(Square square, std::vector<Move>& moves);
    void generateRookMoves(Square square, std::vector<Move>& moves);
    void generateQueenMoves(Square square, std::vector<Move>& moves);
    void generateKingMoves(Square square, std::vector<Move>& moves);

    bool isSquareAttacked(Square square, Side attackerSide) const;
    BitBoard getAttacksForPiece(Piece piece) const;

    void generateSlidingMoves(Square square, const Offset* directions, int numDirections,
                              std::vector<Move>& moves) const;

  private:
    const Board& _board;
};
