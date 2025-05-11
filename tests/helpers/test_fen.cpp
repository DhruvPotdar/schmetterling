#include "board/board.hpp"
#include "helpers/fen.hpp"
#include <gtest/gtest.h>

TEST(FenTest, ParseStartPosition) {
    auto posInfo = Fen::positionFromFen(Fen::startPositionFen);
    EXPECT_TRUE(posInfo.whiteCastleKingside);
    EXPECT_TRUE(posInfo.blackCastleQueenside);
    EXPECT_EQ(posInfo.enPassantFile, 0);
    EXPECT_TRUE(posInfo.whiteToMove);
}

TEST(FenTest, FlipFen) {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string flipped = Fen::flipFen(fen);
    EXPECT_EQ(flipped, "RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr b kqKQ - 0 1");
}

TEST(FenTest, CurrentFen) {
    Board board = Board::createBoard(Fen::startPositionFen);
    Move e2e4(Board::squareIndexFromName("e2"), Board::squareIndexFromName("e4"),
              MoveFlag::PawnTwoUpFlag);
    board.makeMove(e2e4, false);
    EXPECT_EQ(Fen::currentFen(board), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKB1R b KQkq e3 0 1");
}
