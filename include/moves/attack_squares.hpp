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

// Helper function to compute attack bitboard at compile-time
constexpr BitBoard computeKnightAttacks(int squareIndex) {
    uint64_t attacks = 0;
    int file = squareIndex % 8;
    int rank = squareIndex / 8;
    for (const auto& offset : knightOffsets) {
        int newFile = file + offset.file;
        int newRank = rank + offset.rank;
        if (newFile >= 0 && newFile < 8 && newRank >= 0 && newRank < 8) {
            int targetIndex = newFile + newRank * 8;
            attacks |= (1ULL << targetIndex);
        }
    }
    return BitBoard{attacks};
}

static constexpr std::array<BitBoard, 64> knightAttacks = []() {
    std::array<BitBoard, 64> attacks{};
    for (int i = 0; i < 64; ++i) {
        attacks[i] = computeKnightAttacks(i);
    }
    return attacks;
}();

constexpr BitBoard computeKingAttacks(int squareIndex) {
    uint64_t attacks = 0;
    int file = squareIndex % 8;
    int rank = squareIndex / 8;
    for (const auto& offset : kingOffsets) {
        int newFile = file + offset.file;
        int newRank = rank + offset.rank;
        if (newFile >= 0 && newFile < 8 && newRank >= 0 && newRank < 8) {
            int targetIndex = newFile + newRank * 8;
            attacks |= (1ULL << targetIndex);
        }
    }
    return BitBoard{attacks};
}

static constexpr std::array<BitBoard, 64> kingAttacks = []() {
    std::array<BitBoard, 64> attacks{};
    for (int i = 0; i < 64; ++i) {
        attacks[i] = computeKingAttacks(i);
    }
    return attacks;
}();

constexpr BitBoard computeWhitePawnAttacks(int squareIndex) {
    uint64_t attacks = 0;
    int file = squareIndex % 8;
    int rank = squareIndex / 8;
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
    int file = squareIndex % 8;
    int rank = squareIndex / 8;
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
    for (int i = 0; i < 64; ++i) {
        attacks[i] = computeBlackPawnAttacks(i);
    }
    return attacks;
}();

} // namespace AttackTables
