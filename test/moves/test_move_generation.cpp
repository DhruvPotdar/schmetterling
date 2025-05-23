#include "board/board.hpp"
#include "board/squares.hpp"
#include "moves/move_generation.hpp"
#include <gtest/gtest.h>

// Test fixture for MoveGenerator tests
class MoveGeneratorTest : public testing::Test {
  protected:
    void SetUp() override {
        // Initialize a board with the standard starting position
        board = Board(Board::startPositionFen);
        MoveGenerator moveGen(board);
    }

    Board board;
    MoveGenerator moveGen;
    MoveGeneratorTest() : board(Board::startPositionFen), moveGen(board) {}
};

// Test pawn moves from E2 in the starting position
TEST_F(MoveGeneratorTest, PawnMovesFromE2) {
    Square e2("e2");
    auto moves = moveGen.generatePawnMoves(e2);
    std::vector<std::string> moveStrings;
    for (const auto& move : moves) {
        Square from(move.startSquareIndex());
        Square to(move.targetSquareIndex());
        moveStrings.push_back(from.getAlgebraic() + to.getAlgebraic());
    }

    // Expect e2e3 and e2e4 (single and double push)
    std::vector<std::string> expected = {"E2E3", "E2E4"};
    EXPECT_EQ(moves.size(), expected.size());
    for (const auto& move : moveStrings) {

        EXPECT_TRUE(std::find(expected.begin(), expected.end(), move) != expected.end());
    }
}

// Test knight moves from G1 in the starting position
TEST_F(MoveGeneratorTest, KnightMovesFromG1) {
    Square g1("g1");
    auto moves = moveGen.generateKnightMoves(g1);
    std::vector<std::string> moveStrings;
    for (const auto& move : moves) {
        Square from(move.startSquareIndex());
        Square to(move.targetSquareIndex());
        moveStrings.push_back(from.getAlgebraic() + to.getAlgebraic());
    }

    // Expect g1f3, g1h3
    std::vector<std::string> expected = {"G1F3", "G1H3"};
    EXPECT_EQ(moves.size(), expected.size());
    for (const auto& move : moveStrings) {
        EXPECT_TRUE(std::find(expected.begin(), expected.end(), move) != expected.end());
    }
}

// Test king moves from E1 in the starting position
TEST_F(MoveGeneratorTest, KingMovesFromE1) {
    Square e1("E1");
    auto moves = moveGen.generateKingMoves(e1);
    EXPECT_EQ(moves.size(), 0); // No moves in starting position (blocked and no castling yet)
}

// Test pawn promotion from D7 (custom position)
TEST_F(MoveGeneratorTest, PawnPromotion) {
    board = Board("8/3P4/8/8/8/8/8/k6K w - - 0 1");
    MoveGenerator moveGen(board);

    Square d7("d7");
    auto moves = moveGen.generatePawnMoves(d7);
    std::vector<std::string> moveStrings;
    for (const auto& move : moves) {
        Square from(move.startSquareIndex());
        Square to(move.targetSquareIndex());
        std::string moveStr = from.getAlgebraic() + to.getAlgebraic();
        if (move.isPromotion()) {
            moveStr +=
                "=" + std::string(1, move.getPromotionPieceType() == PieceType::Queen    ? 'Q'
                                     : move.getPromotionPieceType() == PieceType::Rook   ? 'R'
                                     : move.getPromotionPieceType() == PieceType::Knight ? 'N'
                                                                                         : 'B');
        }
        moveStrings.push_back(moveStr);
    }

    // Expect d7d8=Q, d7d8=R, d7d8=N, d7d8=B
    std::vector<std::string> expected = {"D7D8=Q", "D7D8=R", "D7D8=N", "D7D8=B"};
    EXPECT_EQ(moves.size(), expected.size());
    for (const auto& move : moveStrings) {
        EXPECT_TRUE(std::find(expected.begin(), expected.end(), move) != expected.end());
    }
}

// Test en passant (custom position)

TEST_F(MoveGeneratorTest, EnPassantCaptures) {
    // Test White en passant
    {
        board = Board("rnbqkbnr/p1pppppp/8/3P4/1pP5/8/PP2PPPP/RNBQKBNR b KQkq c3 0 3");
        MoveGenerator moveGen(board);
        Square square("b4");
        auto moves = moveGen.generatePawnMoves(square);

        std::vector<std::string> moveStrings;
        for (const auto& move : moves) {
            Square from(move.startSquareIndex());
            Square to(move.targetSquareIndex());
            std::string moveStr = from.getAlgebraic() + to.getAlgebraic();
            // if (move.getFlags() == MoveFlag::EnPassantCaptureFlag) {
            //     moveStr += "";
            // }
            moveStrings.push_back(moveStr);
        }

        std::vector<std::string> expected = {"B4B3", "B4C3"};
        EXPECT_EQ(moves.size(), expected.size());
        for (const auto& move : expected) {
            EXPECT_TRUE(std::find(moveStrings.begin(), moveStrings.end(), move) !=
                        moveStrings.end());
        }
    }

    // Test Black en passant
    {
        board = Board("rnbqkbnr/p1pppppp/8/3P4/1pP5/8/PP2PPPP/RNBQKBNR b KQkq c3 0 3");
        MoveGenerator moveGen(board);
        Square d4("b4");
        auto moves = moveGen.generatePawnMoves(d4);
        std::cout << Board::createDiagram(board);
        std::vector<std::string> moveStrings;
        for (const auto& move : moves) {
            Square from(move.startSquareIndex());
            Square to(move.targetSquareIndex());
            std::string moveStr = from.getAlgebraic() + to.getAlgebraic();
            std::cout << moveStr << "Move String\n";
            moveStrings.push_back(moveStr);
        }

        std::vector<std::string> expected = {"B4B3", "B4C3"};
        EXPECT_EQ(moves.size(), expected.size());

        for (const auto& move : expected) {
            EXPECT_TRUE(std::find(moveStrings.begin(), moveStrings.end(), move) !=
                        moveStrings.end());
        }
    }
}
