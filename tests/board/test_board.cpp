#include "board/board.hpp"
#include "helpers/fen.hpp"
#include <gtest/gtest.h>

class BoardTest : public ::testing::Test {
  protected:
    void SetUp() override { board = Board::createBoard(Fen::startPositionFen); }
    Board board;
};

TEST_F(BoardTest, InitialPosition) {
    EXPECT_EQ(board.getCurrentFen(), Fen::startPositionFen);
    EXPECT_FALSE(board.isInCheck());
    EXPECT_TRUE(board.currentGameState.HasKingsideCastleRight(true));
    EXPECT_TRUE(board.currentGameState.HasQueensideCastleRight(false));
}

TEST_F(BoardTest, MakeMoveE2E4) {
    Move e2e4(Board::squareIndexFromName("e2"), Board::squareIndexFromName("e4"),
              MoveFlag::PawnTwoUpFlag);
    board.makeMove(e2e4, false);
    EXPECT_EQ(board.getCurrentFen(), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKB1R b KQkq e3 0 1");
    EXPECT_FALSE(board.isInCheck());
}

TEST_F(BoardTest, UnmakeMove) {
    Move e2e4(Board::squareIndexFromName("e2"), Board::squareIndexFromName("e4"),
              MoveFlag::PawnTwoUpFlag);
    board.makeMove(e2e4, false);
    board.unmakeMove(e2e4, false);
    EXPECT_EQ(board.getCurrentFen(), Fen::startPositionFen);
}

TEST_F(BoardTest, Castling) {
    board = Board::createBoard("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    Move kingside(Board::squareIndexFromName("e1"), Board::squareIndexFromName("g1"),
                  MoveFlag::CastleFlag);
    board.makeMove(kingside, false);
    EXPECT_EQ(board.getCurrentFen(), "r3k2r/8/8/8/8/8/8/R4RK1 b kq - 0 1");
}
