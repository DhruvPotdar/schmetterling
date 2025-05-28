#include "evaluation/evaluation.hpp"
#include "board/bitboard.hpp"
#include "evaluation/pieceSquareTables.hpp"

#include <array>

/**
 * @brief Computes the bitboard mask for squares that must be free of enemy pawns for a pawn to be
 * passed.
 * @param square The square of the pawn.
 * @param side The side of the pawn (White or Black).
 * @return BitBoard of squares in front of the pawn on the same and adjacent files.
 */

BitBoard getPassedPawnMask(Square square, Side side) {
    BitBoard mask;
    const auto file = square.getFile();
    const auto rank = square.getRankIndex();

    if (side == Side::White) {
        for (auto r = rank + 1; r < 8; ++r) {
            if (file > 0) mask.set(Square(file - 1, r));

            mask.set(Square(file, r));

            if (file < 7) mask.set(Square(file + 1, r));
        }
    } else {
        for (auto r = 0; r < rank; ++r) {
            if (file > 0) mask.set(Square(file - 1, r));

            mask.set(Square(file, r));

            if (file < 7) mask.set(Square(file + 1, r));
        }
    }
    return mask;
}

/**
 * @brief Pawn that has advanced beyond all opposing pawns and is not blocked by any enemy pawns
 *
 * @param score
 * @param whitePawns
 * @param blackPawns
 */
void Evaluation::evaluatePassedPawnScore(int& score, const BitBoard& whitePawns,
                                         const BitBoard& blackPawns) {

    for (auto side : {Side::White, Side::Black}) {

        auto pawns = (side == Side::White) ? whitePawns : blackPawns;
        const auto enemyPawns = (side == Side::White) ? blackPawns : whitePawns;

        while (pawns) {
            const auto square = pawns.popLSB();
            const auto mask = getPassedPawnMask(square, side);

            if ((mask & enemyPawns) == 0) {
                score += (side == Side::White) ? passedPawnBonus : -passedPawnBonus;
            }
        }
    }
}

void Evaluation::evaluateIsolatedPawnScore(int& score, const BitBoard& whitePawns,
                                           const BitBoard& blackPawns) {
    // Isolated pawns
    for (Side side : {Side::White, Side::Black}) {
        auto pawns = (side == Side::White) ? whitePawns : blackPawns;
        while (pawns) {

            const auto square = pawns.popLSB();
            const auto file = square.getFile();
            BitBoard adjacentFiles;

            if (file > 0) adjacentFiles |= fileMasks[file - 1];
            if (file < 7) adjacentFiles |= fileMasks[file + 1];

            if ((adjacentFiles & pawns) == 0) {
                score += (side == Side::White) ? -isolatedPawnPenalty : isolatedPawnPenalty;
            }
        }
    }
}

/**
 * @brief Find pawns that are stacked in front of each other(bad) and apply the appropriate penalty
 *
 * @param score
 * @param whitePawns
 * @param blackPawns
 */
void Evaluation::evaluateDoubledPawns(int& score, const BitBoard& whitePawns,
                                      const BitBoard& blackPawns) {
    // Doubled pawns
    for (const auto side : {Side::White, Side::Black}) {
        const auto pawns = (side == Side::White) ? whitePawns : blackPawns;

        for (auto file = 0; file < 8; ++file) {

            const auto filePawns = pawns & fileMasks[file];
            const auto count = filePawns.popCount();

            if (count > 1) {
                const auto penalty = (count - 1) * doubledPawnPenalty;
                score += (side == Side::White) ? -penalty : penalty;
            }
        }
    }
}

/**
 * @brief Computes the pawn structure score, including passed, isolated, and doubled pawns.
 * @param board
 * @return The pawn structure score (positive for White advantage, negative for Black).
 */
int Evaluation::computePawnStructureScore(const Board& board) {
    auto score = 0;
    const auto whitePawns = board.currentState.piecesBitBoards[0];
    const auto blackPawns = board.currentState.piecesBitBoards[6];

    evaluatePassedPawnScore(score, whitePawns, blackPawns);
    evaluateIsolatedPawnScore(score, whitePawns, blackPawns);
    evaluateDoubledPawns(score, whitePawns, blackPawns);

    return score;
}

/**
 * @brief Computes the king safety score based on pawn shield.
 * @param board The current board state.
 * @return The king safety score (positive for White advantage, negative for Black).
 */
int Evaluation::computeKingSafetyScore(const Board& board) {
    auto score = 0;
    for (const auto side : {Side::White, Side::Black}) {
        const auto kingSquare = board.findKingSquare(side);

        if (kingSquare == Square::None) continue;

        const auto file = kingSquare.getFile();
        const auto rank = kingSquare.getRankIndex();
        std::vector<Square> shieldSquares;

        if (side == Side::White && rank < 7) {
            for (auto df = -1; df <= 1; ++df) {

                auto f = file + df;
                if (f >= 0 && f < 8) {
                    shieldSquares.push_back(Square(f, rank + 1));
                }
            }
        } else if (side == Side::Black && rank > 0) {
            for (auto df = -1; df <= 1; ++df) {

                auto f = file + df;
                if (f >= 0 && f < 8) {
                    shieldSquares.push_back(Square(f, rank - 1));
                }
            }
        }

        const auto friendlyPawns =
            board.currentState.piecesBitBoards[side * 6 + static_cast<int>(PieceType::Pawn)];
        auto count = 0;
        for (Square square : shieldSquares) {
            if (friendlyPawns.contains(square)) {
                count++;
            }
        }
        const auto bonus = count * pawnShieldBonus;
        score += (side == Side::White) ? bonus : -bonus;
    }
    return score;
}

/**
 * @brief Evaluate the score based on piece square tables which store the score addons based on the
 * position of a piece on the board
 * @param board
 * @return
 */
int evaluatePieceSquareTables(const Board& board) {
    auto score = 0;
    for (auto pieceIndex = 0; pieceIndex < 12; ++pieceIndex) {

        const auto side = static_cast<Side>(pieceIndex / 6);
        const auto type = static_cast<PieceType>(pieceIndex % 6);
        auto pieceBitBoard = board.currentState.piecesBitBoards[pieceIndex];

        if (type == PieceType::King) {
            const auto table = PieceSquareTables::getKingMiddlegame(side);

            while (pieceBitBoard) {

                const auto square = pieceBitBoard.popLSB();
                score +=
                    (side == Side::White) ? table[square.getIndex()] : -table[square.getIndex()];
            }
        } else {
            const auto table = PieceSquareTables::get(Piece(type, side));
            while (pieceBitBoard) {
                const auto square = pieceBitBoard.popLSB();
                score +=
                    (side == Side::White) ? table[square.getIndex()] : -table[square.getIndex()];
            }
        }
    }
    return score;
}
/**
 * @brief Evaluates the board position based on material, piece-square tables, pawn structure, and
 * king safety.
 *
 * This function computes a score from White's perspective, incorporating:
 * - Material count with bishop pair bonus
 * - Piece-square table bonuses for piece placement
 * - Pawn structure (passed, isolated, doubled pawns)
 * - King safety based on pawn shield
 *
 * @param board The current state of the chess board.
 * @return The evaluation score in centipawns.
 */
int Evaluation::evaluate(const Board& board) {
    auto score = 0;

    // Material evaluation with bishop pair bonus
    for (auto pieceIndex = 0; pieceIndex < 12; ++pieceIndex) {
        const auto side = static_cast<Side>(pieceIndex / 6);
        const auto type = static_cast<PieceType>(pieceIndex % 6);

        if (type == PieceType::King) continue;

        const auto pieceBitBoard = board.currentState.piecesBitBoards[pieceIndex];
        const auto pieceCount = pieceBitBoard.popCount();
        const auto material = materialValues[static_cast<int>(type)];

        score += (side == Side::White) ? pieceCount * material : -pieceCount * material;
    }

    const auto whiteBishopCount = board.currentState.piecesBitBoards[2].popCount();
    const auto blackBishopCount = board.currentState.piecesBitBoards[8].popCount();

    if (whiteBishopCount >= 2) score += bishopPairBonus;
    if (blackBishopCount >= 2) score -= bishopPairBonus;

    // Piece-square tables
    score += evaluatePieceSquareTables(board);

    // Pawn structure
    score += computePawnStructureScore(board);

    // King safety
    score += computeKingSafetyScore(board);

    return score;
}
