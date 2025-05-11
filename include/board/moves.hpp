// Compact (16bit) move representation to preserve memory during search.
//
// The format is as follows (ffffttttttssssss)
// Bits 0-5: <s>tart square index (6 bits = 64 possible squares to check for a
// move) Bits 6-11: <t>arget square index(6 bits = 64 possible squares to check
// for a move)
//
// Bits 12-15: <f>lag (promotion type, etc) [4 bits for flags]

#include "piece.hpp"
#include <cstdint>
typedef uint16_t move_t;
enum class MoveFlag : uint8_t {
    NoFlag = 0b0000,
    EnPassantCaptureFlag = 0b0001,
    CastleFlag = 0b0010,
    PawnTwoUpFlag = 0b0011,
    PromoteToQueenFlag = 0b0100,
    PromoteToKnightFlag = 0b0101,
    PromoteToRookFlag = 0b0110,
    PromoteToBishopFlag = 0b0111
};

class Move {
  private:
    move_t moveValue = 0;
    static constexpr uint16_t startSquareMask = 0x3F;   // 0000000000111111
    static constexpr uint16_t targetSquareMask = 0xFC0; // 0000011111100000
    static constexpr uint16_t flagMask = 0xF000;        // 1111000000000000

  public:
    // Constructor to initialize a Move with a 16-bit value
    Move(move_t moveVal) { moveValue = moveVal; }

    // Constructor to create a quiet move (no flag)
    Move(int startSquare, int targetSquare) {
        moveValue = static_cast<move_t>(startSquare | targetSquare << 6);
    }

    // Constructor to create a move with a flag
    Move(int startSquare, int targetSquare, MoveFlag flag) {
        moveValue =
            // Perform the move and place the flag in as well
            static_cast<move_t>(startSquare | targetSquare << 6 | static_cast<int>(flag) << 12);
    }

    // Getter for the raw 16-bit value
    move_t Value() const { return moveValue; }

    // Checks if the move is considered "null" (value is 0)
    constexpr bool IsNull() const { return moveValue == 0; }

    // Extracts the starting square index
    int StartSquare() const { return moveValue & startSquareMask; }

    // Extracts the target square index
    int targetSquare() const { return (moveValue & targetSquareMask) >> 6; }

    // Checks if the move is a promotion
    bool IsPromotion() const { return GetMoveFlag() >= MoveFlag::PromoteToQueenFlag; }
    // Extracts the move flag
    MoveFlag GetMoveFlag() const { return static_cast<MoveFlag>(moveValue >> 12); }

    int getPromotionPieceType() {
        MoveFlag flag = GetMoveFlag();
        switch (flag) {
        case MoveFlag::PromoteToRookFlag:
            return Piece::Rook;
        case MoveFlag::PromoteToKnightFlag:
            return Piece::Knight;
        case MoveFlag::PromoteToBishopFlag:
            return Piece::Bishop;
        case MoveFlag::PromoteToQueenFlag:
            return Piece::Queen;
        default:
            return Piece::None;
        }
    }
};
