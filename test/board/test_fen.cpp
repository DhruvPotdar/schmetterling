#include "board/board.hpp"
#include "board/fen.hpp"
#include <gtest/gtest.h>

TEST(FENTest, ParseAndGenerate) {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Board board;
    FEN::parse(fen, board);

    EXPECT_EQ(board.getPieceAt(Square("A1")).type, PieceType::Rook);
    EXPECT_EQ(board.getPieceAt(Square("E2")).type, PieceType::Pawn);
    EXPECT_EQ(board.getPieceAt(Square("E4")).type, PieceType::None);

    EXPECT_EQ(board.side, Side::White);

    std::string generatedFen = FEN::generate(board);
    EXPECT_EQ(generatedFen, fen);
}
