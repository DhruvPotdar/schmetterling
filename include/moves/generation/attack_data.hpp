#pragma once

#include "board/board.hpp"
#include "moves/generation/attack_squares.hpp"

// Lightweight scaffolding for mask-driven legal move generation.

struct AttackData {
    Square kingSq{Square::None};
    BitBoard oppAttackMap{0};     // Squares attacked by opponent
    BitBoard pinRayMask{0};       // Friendly pieces that are pinned (bit set if pinned)
    BitBoard checkRayMask{~BitBoard(0)}; // Squares that resolve check (all-ones if not in check)
    bool inCheck{false};
    bool doubleCheck{false};
};

// Returns bitboard of attackers (of "attackerSide") to the given square under the provided occupancy.
BitBoard getAttackersTo(const Board& board, Square square, Side attackerSide, BitBoard occupied);

// Computes AttackData for sideToMove from current board state.
AttackData calculateAttackData(const Board& board, Side sideToMove);


