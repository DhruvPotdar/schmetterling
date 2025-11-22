#pragma once

#include "board/bitboard.hpp"
#include "board/board.hpp"
#include "moves/moves.hpp"
#include <system_error>
#include <vector>

/**
 * @class MoveGenerator
 * @brief Takes in the board and generates all possible legal and pseudo legal moves for all pieces
 * on the board
 */
class MoveGen {

  public:
    MoveGen();
    MoveGen(const Board& board) : _board(board) {};
    const std::vector<Move> generateLegalMoves();

  private:
    const Board& _board;
};
