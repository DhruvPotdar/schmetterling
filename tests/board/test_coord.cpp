#include "board/coord.hpp"
#include <gtest/gtest.h>

TEST(CoordTest, Constructor) {
    Coord coord(3, 4); // d5
    EXPECT_EQ(coord.x, 3);
    EXPECT_EQ(coord.y, 4);
}

TEST(CoordTest, SquareIndex) {
    Coord coord(0, 0); // a1
    EXPECT_EQ(coord.squareIndex(), 0);
    Coord h8(7, 7); // h8
    EXPECT_EQ(h8.squareIndex(), 63);
}

TEST(CoordTest, IsValidSquare) {
    Coord valid(4, 4); // e5
    EXPECT_TRUE(valid.isValidSquare());
    Coord invalid(-1, 0);
    EXPECT_FALSE(invalid.isValidSquare());
}

TEST(CoordTest, OperatorPlus) {
    Coord a(2, 3);        // c4
    Coord b(1, 1);        // b2
    Coord result = a + b; // d6
    EXPECT_EQ(result.x, 3);
    EXPECT_EQ(result.y, 4);
}

TEST(CoordTest, IsLightSquare) {
    Coord b1(1, 0); // b1
    EXPECT_TRUE(b1.isLightSquare());
    Coord a1(0, 0); // a1
    EXPECT_FALSE(a1.isLightSquare());
}
