#pragma once

#include "board/bitboard.hpp"
#include <array>

namespace AttackTables {

static constexpr Offset knightOffsets[] = {Offset(-2, -1), Offset(-2, 1), Offset(-1, -2),
                                           Offset(-1, 2),  Offset(1, -2), Offset(1, 2),
                                           Offset(2, -1),  Offset(2, 1)};

static constexpr Offset kingOffsets[] = {Offset(-1, -1), Offset(-1, 0), Offset(-1, 1),
                                         Offset(0, -1),  Offset(0, 1),  Offset(1, -1),
                                         Offset(1, 0),   Offset(1, 1)};

static constexpr Offset bishopOffsets[] = {Offset(-1, -1), Offset(-1, 1), Offset(1, -1),
                                           Offset(1, 1)};
static constexpr Offset rookOffsets[] = {Offset(-1, 0), Offset(1, 0), Offset(0, -1), Offset(0, 1)};
static constexpr Offset queenOffsets[] = {Offset(-1, -1), Offset(-1, 1), Offset(1, -1),
                                          Offset(1, 1),   Offset(-1, 0), Offset(1, 0),
                                          Offset(0, -1),  Offset(0, 1)};

// Precomputed occupancy masks for each square and direction
constexpr BitBoard computeRayMask(Square square, const Offset& direction) {
    BitBoard mask;
    const auto file = square.getFile();
    const auto rank = square.getRankIndex();
    auto currentFile = file + direction.file;
    auto currentRank = rank + direction.rank;

    while (currentFile >= 0 && currentFile < 8 && currentRank >= 0 && currentRank < 8) {
        mask.set(currentFile + currentRank * 8);
        currentFile += direction.file;
        currentRank += direction.rank;
    }
    return mask;
}

// Precomputed ray masks for each square in each direction
static constexpr std::array<std::array<BitBoard, 8>, 64> rayMasks = []() {
    std::array<std::array<BitBoard, 8>, 64> masks{};
    for (int square = 0; square < 64; ++square) {
        // Diagonal rays
        masks[square][0] = computeRayMask(square, bishopOffsets[0]); // NorthWest
        masks[square][1] = computeRayMask(square, bishopOffsets[1]); // NorthEast
        masks[square][2] = computeRayMask(square, bishopOffsets[2]); // SouthWest
        masks[square][3] = computeRayMask(square, bishopOffsets[3]); // SouthEast

        // Orthogonal rays
        masks[square][4] = computeRayMask(square, rookOffsets[0]); // North
        masks[square][5] = computeRayMask(square, rookOffsets[1]); // South
        masks[square][6] = computeRayMask(square, rookOffsets[2]); // West
        masks[square][7] = computeRayMask(square, rookOffsets[3]); // East
    }
    return masks;
}();

// Precomputed bishop masks (all diagonal rays combined)
static constexpr std::array<BitBoard, 64> bishopMasks = []() {
    std::array<BitBoard, 64> masks{};
    for (int square = 0; square < 64; ++square) {
        masks[square] =
            rayMasks[square][0] | rayMasks[square][1] | rayMasks[square][2] | rayMasks[square][3];
    }
    return masks;
}();

// Precomputed rook masks (all orthogonal rays combined)
static constexpr std::array<BitBoard, 64> rookMasks = []() {
    std::array<BitBoard, 64> masks{};
    for (int square = 0; square < 64; ++square) {
        masks[square] =
            rayMasks[square][4] | rayMasks[square][5] | rayMasks[square][6] | rayMasks[square][7];
    }
    return masks;
}();

// Helper function to compute attack bitboard at compile-time
constexpr BitBoard computeKnightAttacks(int squareIndex) {
    uint64_t attacks = 0;
    const auto file = squareIndex % 8;
    const auto rank = squareIndex / 8;
    for (const auto& offset : knightOffsets) {
        const auto newFile = file + offset.file;
        const auto newRank = rank + offset.rank;
        if (newFile >= 0 && newFile < 8 && newRank >= 0 && newRank < 8) {
            const auto targetIndex = newFile + newRank * 8;
            attacks |= (1ULL << targetIndex);
        }
    }
    return BitBoard{attacks};
}

static constexpr std::array<BitBoard, 64> knightAttacks = []() {
    std::array<BitBoard, 64> attacks{};
    for (auto i = 0; i < 64; ++i) {
        attacks[i] = computeKnightAttacks(i);
    }
    return attacks;
}();

constexpr BitBoard computeKingAttacks(int squareIndex) {
    uint64_t attacks = 0;
    const auto file = squareIndex % 8;
    const auto rank = squareIndex / 8;
    for (const auto& offset : kingOffsets) {
        const auto newFile = file + offset.file;
        const auto newRank = rank + offset.rank;
        if (newFile >= 0 && newFile < 8 && newRank >= 0 && newRank < 8) {
            const auto targetIndex = newFile + newRank * 8;
            attacks |= (1ULL << targetIndex);
        }
    }
    return BitBoard{attacks};
}

static constexpr std::array<BitBoard, 64> kingAttacks = []() {
    std::array<BitBoard, 64> attacks{};
    for (auto i = 0; i < 64; ++i) {
        attacks[i] = computeKingAttacks(i);
    }
    return attacks;
}();

constexpr BitBoard computeWhitePawnAttacks(int squareIndex) {
    uint64_t attacks = 0;
    const auto file = squareIndex % 8;
    const auto rank = squareIndex / 8;
    // Up-left: file - 1, rank + 1
    if (file > 0 && rank < 7) {
        attacks |= (1ULL << (squareIndex + 7));
    }
    // Up-right: file + 1, rank + 1
    if (file < 7 && rank < 7) {
        attacks |= (1ULL << (squareIndex + 9));
    }
    return BitBoard{attacks};
}

static constexpr std::array<BitBoard, 64> whitePawnAttacks = []() {
    std::array<BitBoard, 64> attacks{};
    for (int i = 0; i < 64; ++i) {
        attacks[i] = computeWhitePawnAttacks(i);
    }
    return attacks;
}();

constexpr BitBoard computeBlackPawnAttacks(int squareIndex) {
    uint64_t attacks = 0;
    const auto file = squareIndex % 8;
    const auto rank = squareIndex / 8;
    // Down-left: file - 1, rank - 1
    if (file > 0 && rank > 0) {
        attacks |= (1ULL << (squareIndex - 9));
    }
    // Down-right: file + 1, rank - 1
    if (file < 7 && rank > 0) {
        attacks |= (1ULL << (squareIndex - 7));
    }
    return BitBoard{attacks};
}

static constexpr std::array<BitBoard, 64> blackPawnAttacks = []() {
    std::array<BitBoard, 64> attacks{};
    for (auto i = 0; i < 64; ++i) {
        attacks[i] = computeBlackPawnAttacks(i);
    }
    return attacks;
}();

} // namespace AttackTables
