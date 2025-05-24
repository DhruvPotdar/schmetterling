#include "board/board.hpp"
#include "board/squares.hpp"
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
    std::string fen = "rnbqkbnr/ppp1p1pp/3p4/5p1Q/4P3/8/PPPP1PPP/RNB1KBNR b KQkq - 1 3";
    Board board(fen);
    EXPECT_TRUE(board.isInCheck());
}

TEST(BoardTest, CastlingKingsideWhite) {
    Board board("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    Square from("E1");
    Square to("G1");

    auto undoInfo = board.makeMove(from, to);
    EXPECT_EQ(board.getPieceAt(Square("G1")).type, PieceType::King);
    EXPECT_EQ(board.getPieceAt(Square("F1")).type, PieceType::Rook);
    EXPECT_EQ(board.getPieceAt(Square("E1")).type, PieceType::None);
    EXPECT_EQ(board.getPieceAt(Square("H1")).type, PieceType::None);

    board.unMakeMove(from, to, undoInfo);
    EXPECT_EQ(board.getPieceAt("E1").type, PieceType::King);
    EXPECT_EQ(board.getPieceAt("H1").type, PieceType::Rook);
}

TEST(BoardTest, CastlingQueensideBlack) {
    Board board("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
    Square from("E8");
    Square to("C8");

    // std::cout << Board::createDiagram(board);
    auto undoInfo = board.makeMove(from, to);

    EXPECT_EQ(board.getPieceAt("C8").type, PieceType::King);
    EXPECT_EQ(board.getPieceAt("D8").type, PieceType::Rook);
    EXPECT_EQ(board.getPieceAt("E8").type, PieceType::None);
    EXPECT_EQ(board.getPieceAt("A8").type, PieceType::None);

    // std::cout << Board::createDiagram(board);
    board.unMakeMove(from, to, undoInfo);
    EXPECT_EQ(board.getPieceAt("E8").type, PieceType::King);
    EXPECT_EQ(board.getPieceAt("A8").type, PieceType::Rook);
}

TEST(BoardTest, EnPassantCapture) {
    Board board("8/8/8/3pP3/8/8/8/8 w - d6 0 1");

    Square from("E5");
    Square to("D6");
    auto undoInfo = board.makeMove(from, to);

    EXPECT_EQ(board.getPieceAt("D6").type, PieceType::Pawn);
    EXPECT_EQ(board.getPieceAt("D5").type, PieceType::None); // Captured pawn
    EXPECT_EQ(board.getPieceAt("E5").type, PieceType::None);

    board.unMakeMove(from, to, undoInfo);
    EXPECT_EQ(board.getPieceAt("E5").type, PieceType::Pawn);
    EXPECT_EQ(board.getPieceAt("D5").type, PieceType::Pawn);
    EXPECT_EQ(board.getPieceAt("D6").type, PieceType::None);
}

// TEST(BoardTest, PawnPromotionToQueen) {
//     Board board("8/P7/8/8/8/8/8/8 w - - 0 1");
//     Square from("A7");
//     Square to("A8");
//     auto undoInfo = board.makeMove(from, to, PieceType::Queen);
//
//     EXPECT_EQ(board.getPieceAt("A8").type, PieceType::Queen);
//     EXPECT_EQ(board.getPieceAt("A7").type, PieceType::None);
//
//     board.unMakeMove(from, to, undoInfo);
//     EXPECT_EQ(board.getPieceAt("A7").type, PieceType::Pawn);
//     EXPECT_EQ(board.getPieceAt("A8").type, PieceType::None);
// }
//
