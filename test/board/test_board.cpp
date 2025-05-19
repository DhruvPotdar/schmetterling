#include "board/board.hpp"
#include "moves/moves.hpp"
#include <gtest/gtest.h>

TEST(BoardTest, MakeAndUnmakeMove) {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    Square from("E2");
    Square to("E4");
    auto undoInfo = board.makeMove(from, to);

    EXPECT_EQ(board.getPieceAt(from).type, PieceType::None);
    EXPECT_EQ(board.getPieceAt(to).type, PieceType::Pawn);

    board.unMakeMove(from, to, undoInfo);

    EXPECT_EQ(board.getPieceAt(from).type, PieceType::Pawn);
    EXPECT_EQ(board.getPieceAt(to).type, PieceType::None);
}

TEST(BoardTest, InCheck) {
    std::string fen = "rnb1kbnr/pppp1ppp/8/4p3/4P3/8/PPPPKPPP/RNBQ1BNR b kq - 0 2";
    Board board(fen);
    EXPECT_TRUE(board.isInCheck());
}
