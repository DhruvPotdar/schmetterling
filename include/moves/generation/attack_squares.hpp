#pragma once

#include "board/bitboard.hpp"
#include <array>
#include <cstdint>

namespace AttackTables {

// Pre-computed ray attacks for sliding pieces
static constexpr int NUM_RAYS = 8;
static constexpr int NUM_SQUARES = 64;

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
constexpr BitBoard computeKnightAttacks(Square square) {
    uint64_t attacks = 0;
    const auto file = square.getFile();
    const auto rank = square.getRankIndex();
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

constexpr BitBoard computeKingAttacks(Square squareIndex) {
    uint64_t attacks = 0;
    int file = squareIndex.getFile();
    int rank = squareIndex.getRankIndex();
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

constexpr BitBoard computeWhitePawnAttacks(Square square) {
    uint64_t attacks = 0;
    int file = square.getFile();
    int rank = square.getRankIndex();
    // Up-left: file - 1, rank + 1
    if (file > 0 && rank < 7) {
        attacks |= (1ULL << (square.getIndex() + 7));
    }
    // Up-right: file + 1, rank + 1
    if (file < 7 && rank < 7) {
        attacks |= (1ULL << (square.getIndex() + 9));
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

constexpr BitBoard computeBlackPawnAttacks(Square square) {
    uint64_t attacks = 0;
    int file = square.getFile();
    int rank = square.getRankIndex();
    // Down-left: file - 1, rank - 1
    if (file > 0 && rank > 0) {
        attacks |= (1ULL << (square.getIndex() - 9));
    }
    // Down-right: file + 1, rank - 1
    if (file < 7 && rank > 0) {
        attacks |= (1ULL << (square.getIndex() - 7));
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

// Helper function to compute sliding piece attacks along a direction
constexpr BitBoard computeRayAttacks(Square squareIndex, const Offset& direction) {
    uint64_t attacks = 0;
    int file = squareIndex.getFile();
    int rank = squareIndex.getRankIndex();

    int newFile = file + direction.file;
    int newRank = rank + direction.rank;

    while (newFile >= 0 && newFile < 8 && newRank >= 0 && newRank < 8) {
        int targetIndex = newFile + newRank * 8;
        attacks |= (1ULL << targetIndex);
        newFile += direction.file;
        newRank += direction.rank;
    }
    return BitBoard{attacks};
}

// Pre-computed ray attacks for bishops
static constexpr std::array<std::array<BitBoard, NUM_RAYS / 2>, NUM_SQUARES> bishopRayAttacks =
    []() {
        std::array<std::array<BitBoard, NUM_RAYS / 2>, NUM_SQUARES> attacks{};
        for (int square = 0; square < NUM_SQUARES; ++square) {
            for (int direction = 0; direction < NUM_RAYS / 2; ++direction) {
                attacks[square][direction] = computeRayAttacks(square, bishopOffsets[direction]);
            }
        }
        return attacks;
    }();

// Pre-computed ray attacks for rooks
static constexpr std::array<std::array<BitBoard, NUM_RAYS / 2>, NUM_SQUARES> rookRayAttacks = []() {
    std::array<std::array<BitBoard, NUM_RAYS / 2>, NUM_SQUARES> attacks{};
    for (int square = 0; square < NUM_SQUARES; ++square) {
        for (int direction = 0; direction < NUM_RAYS / 2; ++direction) {
            attacks[square][direction] = computeRayAttacks(square, rookOffsets[direction]);
        }
    }
    return attacks;
}();
//
// Direction increases/decreases square index along rays for trimming beyond nearest blocker
static constexpr std::array<bool, 4> bishopDirIncreases = {true, true, false,
                                                           false}; // NW, NE, SW, SE
static constexpr std::array<bool, 4> rookDirIncreases = {true, false, false,
                                                         true}; // N, S, W, E (index: +8,-8,-1,+1)

inline int msbIndex(BitBoard bb) {
    int idx = -1;
    while (bb) {
        Square s(bb.popLSB());
        idx = s.getIndex();
    }
    return idx;
}

inline BitBoard getBishopAttacks(Square square, BitBoard occupied) {
    BitBoard result{0};
    for (int d = 0; d < 4; ++d) {
        BitBoard ray = bishopRayAttacks[square.getIndex()][d];
        BitBoard blockers = ray & occupied;
        if (blockers) {
            int blockerIdx = bishopDirIncreases[d] ? blockers.LSBIndex() : msbIndex(blockers);
            // include up to blocker (inclusive), so remove squares beyond blocker along the same
            // ray
            result |= ray & ~bishopRayAttacks[blockerIdx][d];
        } else {
            result |= ray;
        }
    }
    return result;
}

inline BitBoard getRookAttacks(Square square, BitBoard occupied) {
    BitBoard result{0};
    for (int d = 0; d < 4; ++d) {
        BitBoard ray = rookRayAttacks[square.getIndex()][d];
        BitBoard blockers = ray & occupied;
        if (blockers) {
            int blockerIdx = rookDirIncreases[d] ? blockers.LSBIndex() : msbIndex(blockers);
            result |= ray & ~rookRayAttacks[blockerIdx][d];
        } else {
            result |= ray;
        }
    }
    return result;
}

inline BitBoard getQueenAttacks(Square square, BitBoard occupied) {
    return getBishopAttacks(square, occupied) | getRookAttacks(square, occupied);
}

} // namespace AttackTables
