#pragma once

#include "board/board.hpp"
/**
 * @class Evaluation
 * @brief Provides static methods to evaluate a chess position.
 *
 * This class computes a score for a given board position, incorporating material evaluation,
 * piece-square tables, pawn structure analysis, and king safety. It is designed for extensibility
 * to include additional features like mobility or advanced heuristics.
 */
class Evaluation {
  public:
    /**
     * @brief Evaluates the given board position.
     * @param board The current state of the chess board.
     * @return An integer score in centipawns, where positive values favor White and negative favor
     * Black.
     */
    static int evaluate(const Board& board);

  private:
    // Material values in centipawns (100 = 1 pawn) indexed by piecetype
    static constexpr std::array<int, 6> materialValues = {
        100, // Pawn
        320, // Knight
        330, // Bishop
        500, // Rook
        900, // Queen
        0    // King (no material value)
    };

    static constexpr int bishopPairBonus = 50;     // Bonus for having two bishops
    static constexpr int passedPawnBonus = 100;    // Bonus for each passed pawn
    static constexpr int isolatedPawnPenalty = 20; // Penalty for each isolated pawn
    static constexpr int doubledPawnPenalty = 10;  // Penalty per extra pawn in a doubled stack
    static constexpr int pawnShieldBonus = 10;     // Bonus per pawn in king's shield

    // File masks for pawn structure analysis (all squares in each file)
    static constexpr std::array<BitBoard, 8> fileMasks = []() {
        std::array<BitBoard, 8> masks{};
        for (auto file = 0; file < 8; ++file) {
            for (auto rank = 0; rank < 8; ++rank) {
                masks[file].set(Square(file, rank));
            }
        }
        return masks;
    }();

    static void evaluatePassedPawnScore(int& score, const BitBoard& whitePawns,
                                        const BitBoard& blackPawns);
    static int computeKingSafetyScore(const Board& board);
    static int computePawnStructureScore(const Board& board);
    static void evaluateDoubledPawns(int& score, const BitBoard& whitePawns,
                                     const BitBoard& blackPawns);
    static void evaluateIsolatedPawnScore(int& score, const BitBoard& whitePawns,
                                          const BitBoard& blackPawns);
};
