#pragma once
#include "../board/types.hpp"
#include <cstdint>

enum class MoveFlag {
    NoFlag = 0b0000,
    EnPassantCaptureFlag = 0b0001,
    CastleFlag = 0b0010,
    PawnTwoUpFlag = 0b0011,
    PromoteToQueenFlag = 0b0100,
    PromoteToKnightFlag = 0b0101,
    PromoteToRookFlag = 0b0110,
    PromoteToBishopFlag = 0b0111
};

// Compact (16bit) move representation to preserve memory during search.
//
// The format is as follows (ffffttttttssssss)
//
// - Bits 0-5: [s]tart square index (6 bits = 64 possible squares to check for a
// move)
//
// - Bits 6-11: [t]arget square index(6 bits = 64 possible squares to check
// for a move)
//
// - Bits 12-15: [f]lag (promotion type, etc) [4 bits for flags]
class Move {
  public:
    Move(uint16_t moveVal) { moveValue = moveVal; }

    Move(int startSquare, int targetSquare) { moveValue = (startSquare | targetSquare << 6); }

    Move(int startSquare, int targetSquare, MoveFlag flag) {
        moveValue = (startSquare | targetSquare << 6 | static_cast<int>(flag) << 12);
    }

    // Getter for the raw 16-bit value
    uint16_t value() const { return moveValue; }

    // Checks if the move is considered "null" (value is 0)
    constexpr bool isNull() const { return moveValue == 0; }

    // Extracts the starting square index
    int startSquareIndex() const { return moveValue & startSquareMask; }

    // Extracts the target square index
    int targetSquareIndex() const { return (moveValue & targetSquareMask) >> 6; }

    // Checks if the move is a promotion
    bool isPromotion() const { return getMoveFlag() >= MoveFlag::PromoteToQueenFlag; }

    bool isEnPassant() const { return getMoveFlag() == MoveFlag::EnPassantCaptureFlag; }

    // Extracts the move flag
    MoveFlag getMoveFlag() const { return static_cast<MoveFlag>(moveValue >> 12); }

    PieceType getPromotionPieceType() {
        const auto flag = getMoveFlag();
        switch (flag) {
        case MoveFlag::PromoteToRookFlag:
            return PieceType::Rook;
        case MoveFlag::PromoteToKnightFlag:
            return PieceType::Knight;
        case MoveFlag::PromoteToBishopFlag:
            return PieceType::Bishop;
        case MoveFlag::PromoteToQueenFlag:
            return PieceType::Queen;
        default:
            return PieceType::None;
        }
    }

  private:
    int moveValue = 0;

    static constexpr uint16_t startSquareMask = 0x3F;   // 0000000000111111
    static constexpr uint16_t targetSquareMask = 0xFC0; // 0000011111100000
    static constexpr uint16_t flagMask = 0xF000;        // 1111000000000000
};
