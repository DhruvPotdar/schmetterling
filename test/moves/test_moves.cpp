#include "moves/moves.hpp"
#include <gtest/gtest.h>

TEST(MoveTest, BasicMove) {
    Move move(0, 8); // A1 to A2
    EXPECT_EQ(move.startSquareIndex(), 0);
    EXPECT_EQ(move.targetSquareIndex(), 8);
    EXPECT_FALSE(move.isPromotion());
}

TEST(MoveTest, PromotionMove) {
    Move move(48, 56, MoveFlag::PromoteToQueenFlag); // A7 to A8, promote to queen
    EXPECT_EQ(move.startSquareIndex(), 48);
    EXPECT_EQ(move.targetSquareIndex(), 56);
    EXPECT_TRUE(move.isPromotion());
    EXPECT_EQ(move.getPromotionPieceType(), PieceType::Queen);
}

TEST(MoveTest, EnPassantMove) {
    Move move(32, 40, MoveFlag::EnPassantCaptureFlag); // A5 to A6, en passant
    EXPECT_TRUE(move.isEnPassant());
}
