#include "board/squares.hpp"
#include <gtest/gtest.h>

TEST(SquareTest, Constructors) {
    Square sq1(0); // A1
    EXPECT_EQ(sq1.getFile(), 0);
    EXPECT_EQ(sq1.getRankIndex(), 0);

    Square sq2(7, 7); // H8
    EXPECT_EQ(sq2.getFile(), 7);
    EXPECT_EQ(sq2.getRankIndex(), 7);

    Square sq3("A1");
    EXPECT_EQ(sq3.getIndex(), 0);

    Square sq4("H8");
    EXPECT_EQ(sq4.getIndex(), 63);
}

TEST(SquareTest, Algebraic) {
    Square sq(0);
    EXPECT_EQ(sq.getAlgebraic(), "A1");

    Square sq2(63);
    EXPECT_EQ(sq2.getAlgebraic(), "H8");
}

TEST(SquareTest, Offset) {
    Square sq(0); // A1
    Offset offset(1, 1);
    Square sq2 = sq.offset(offset); // B2
    EXPECT_EQ(sq2.getIndex(), 9);
}

TEST(SquareTest, Distance) {
    Square sq1(0);                      // A1
    Square sq2(63);                     // H8
    EXPECT_EQ(sq1.distanceTo(sq2), 14); // Manhattan distance
}
