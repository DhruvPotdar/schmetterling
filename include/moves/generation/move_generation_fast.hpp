#pragma once

#include "board/board.hpp"
#include "moves/generation/attack_data.hpp"
#include "moves/moves.hpp"

// Implementations will use AttackData to enforce legality without make-move.

class MoveGeneratorFast {
  public:
    explicit MoveGeneratorFast(const Board& board) : _board(board) {}

    // Strictly legal moves for side-to-move
    std::vector<Move> generateLegalMoves();

    // Forcing-only legal moves (captures, promotions, and optionally quiet checks)
    std::vector<Move> generateForcingMoves();

    // Pseudo-legal moves (for perft/divide and quick counts)
    std::vector<Move> generatePseudoLegalMoves();

  private:
    const Board& _board;

    // Piece-specific helpers (to be implemented)
    void generateLegalKingMoves(const AttackData& attack_data, std::vector<Move>& out,
                                bool forcingOnly);
    void generateLegalKnightMoves(const AttackData& attack_data, std::vector<Move>& out,
                                  bool forcingOnly);
    void generateLegalSlidingMoves(PieceType pieceType, const AttackData& ad,
                                   std::vector<Move>& out, bool forcingOnly);
    void generateLegalPawnMoves(const AttackData& attack_data, std::vector<Move>& out,
                                bool forcingOnly);

    // Optional: fast check detection for forcing quiet checks
    bool isMoveACheck(const Move m, int opponentKingSq) const;
};

