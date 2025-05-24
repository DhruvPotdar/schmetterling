#include "board/bitboard.hpp"
#include "board/squares.hpp"
#include <gtest/gtest.h>

TEST(BitBoardTest, DefaultConstructor) {
    BitBoard bb;
    EXPECT_EQ(static_cast<uint64_t>(bb), 0ULL);
}

TEST(BitBoardTest, ExplicitConstructor) {
    BitBoard bb(0xFFULL);
    EXPECT_EQ(static_cast<uint64_t>(bb), 0xFFULL);
}

TEST(BitBoardTest, BitwiseOperations) {
    BitBoard bb1(0xF0ULL);
    BitBoard bb2(0x0FULL);
    EXPECT_EQ(static_cast<uint64_t>(bb1 & bb2), 0ULL);
    EXPECT_EQ(static_cast<uint64_t>(bb1 | bb2), 0xFFULL);
    EXPECT_EQ(static_cast<uint64_t>(bb1 ^ bb2), 0xFFULL);
}

TEST(BitBoardTest, SetClearToggle) {
    BitBoard bb;
    Square sq(0); // A1
    bb.set(sq);
    EXPECT_TRUE(bb.contains(sq));
    bb.clear(sq);
    EXPECT_FALSE(bb.contains(sq));
    bb.toggle(sq);
    EXPECT_TRUE(bb.contains(sq));
}

TEST(BitBoardTest, Shift) {
    BitBoard bb(1ULL);              // A1
    BitBoard shifted = bb.shift(8); // Shift up one rank
    EXPECT_EQ(static_cast<uint64_t>(shifted), 1ULL << 8);
}

TEST(BitBoardTest, OfSquare) {
    Square sq(0); // A1
    BitBoard bb = BitBoard::of(sq);
    EXPECT_EQ(static_cast<uint64_t>(bb), 1ULL);
}

TEST(BitBoardTest, FullBoardOperations) {
    BitBoard fullBoard(~0ULL); // All squares occupied
    EXPECT_EQ(fullBoard.popCount(), 64);
    EXPECT_TRUE(fullBoard.contains(Square("H8")));

    fullBoard.toggle(Square("E4"));
    EXPECT_EQ(fullBoard.popCount(), 63); // Verify single bit clear
}

TEST(BitBoardTest, BoundaryShifts) {
    BitBoard northEdge(0xFF00000000000000); // 8th rank
    BitBoard shifted = northEdge.shift(8);  // Attempt to shift off board
    EXPECT_EQ(shifted, 0ULL);               // Should vanish

    BitBoard southEdge(0xFF); // 1st rank
    BitBoard southShifted = southEdge.shift(-8);
    EXPECT_EQ(southShifted, 0ULL);
}
