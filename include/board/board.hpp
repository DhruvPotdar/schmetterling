#pragma once
#include "board/pieceList.hpp"
#include "moves.hpp"

#include <cstdint>
#include <vector>
typedef uint64_t board;

// Represents the current state of the board during a game.
// The state includes things such as: positions of all pieces, side to move,
// castling rights, en-passant square, etc. Some extra information is included
// as well to help with evaluation and move generation.

// The initial state of the board can be set from a FEN string, and moves are
// subsequently made (or undone) using the MakeMove and UnmakeMove functions.
class Board {
public:
  int WhiteIndex = 0;
  int blackIndex = 1;

  // Stores piece code for each square on the board
  static std::vector<int> square;

  // Square Index of black and white king
  static std::vector<int> kingSquare;

  // Bitboards
  // Vector containing a bitboard For Each Piece type and color(i.e. black and
  // white)
  std::vector<board> piecesBitBoards;

  // Bitboards for all pieces of either colour (all white pieces, all black
  // pieces)
  std::vector<board> colourBitboards;
  board allPiecesBitboard;
  // Pieces by their movement abilities
  board FriendlyOrthogonalSliders;
  board FriendlyDiagonalSliders;
  board EnemyOrthogonalSliders;
  board EnemyDiagonalSliders;
  // Piece count excluding pawns and kings
  int TotalPieceCountWithoutPawnsAndKings;
  // # Piece lists
  PieceList Bishops;
  PieceList Queens;
  PieceList Knights;
  PieceList Pawns;
  // # Side to move info
  bool IsWhiteToMove;
  int moveColor, opponentColor, moveColorIndex, opponentColorIndex;
  int getMoveColor() { return IsWhiteToMove ? Piece::White : Piece::Black; }
  int getOpponentColor() { return IsWhiteToMove ? Piece::Black : Piece::White; }
  int getOpponentMoveColorIndex() {
    return IsWhiteToMove ? WhiteIndex : blackIndex;
  }
  int getMoveColorIndex() { return IsWhiteToMove ? WhiteIndex : blackIndex; }

  void makeMove(Move move);
};
