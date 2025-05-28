#include "board/board.hpp"
#include "evaluation/evaluation.hpp"
#include <gtest/gtest.h>

// Test fixture for Evaluation tests
class EvaluationTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Common setup if needed (e.g., initialize PieceSquareTables if required)
    }
};

// Helper function to evaluate a position from FEN
int evaluatePosition(const std::string& fen) {
    Board board(fen);
    return Evaluation::evaluate(board);
}

// Test 1: Starting position (balanced, should evaluate to 0)
TEST_F(EvaluationTest, StartingPosition) {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    int score = evaluatePosition(fen);
    EXPECT_EQ(score, 0) << "Starting position should evaluate to 0 (balanced).";
}

// Test 2: Material imbalance (White missing a queen)
TEST_F(EvaluationTest, MaterialImbalance_WhiteNoQueen) {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNB1KBNR w KQkq - 0 1";
    int score = evaluatePosition(fen);
    int expected = -900; // Queen = 900 centipawns
    EXPECT_EQ(score, expected) << "White missing a queen should evaluate to -900.";
}

// Test 3: Bishop pair bonus (White has bishop pair, Black doesn’t)
TEST_F(EvaluationTest, BishopPairBonus) {
    std::string fen = "rnbqk1nr/pppp1ppp/5n2/8/8/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1";
    int score = evaluatePosition(fen);
    int expected = 50; // Bishop pair bonus for White
    EXPECT_NEAR(score, expected, 20) << "White’s bishop pair should give ~50 centipawn advantage.";
}

// Test 4: Piece-square tables (knights centralized vs. corner)
TEST_F(EvaluationTest, PieceSquareTables_Knights) {
    std::string fen =
        "rnbqkbnr/pppppppp/8/8/3NN3/8/PPPPPPPP/R1BQKB1R w KQkq - 0 1"; // Knights on D4, E4
    int centralized = evaluatePosition(fen);
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RN1QKB1N w KQkq - 0 1"; // Knights on A1, H1
    int corner = evaluatePosition(fen);
    EXPECT_GT(centralized, corner) << "Centralized knights score higher than corner knights.";
}

// Test 5: Passed pawn (White has a passed pawn on D5)
TEST_F(EvaluationTest, PawnStructure_PassedPawn) {
    std::string fen = "rnbqkbnr/ppp1pppp/5n2/8/3P4/5N2/PPP1PPPP/RNBQKB1R b KQkq - 0 1";
    int score = evaluatePosition(fen);
    int expected = 100; // Passed pawn bonus
    EXPECT_NEAR(score, expected, 20) << "White’s passed pawn on D5 should give ~100 advantage.";
}

// Test 6: Isolated pawn (White has an isolated pawn on D2)
TEST_F(EvaluationTest, PawnStructure_IsolatedPawn) {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PP1P1PPP/RNBQKBNR w KQkq - 0 1";
    int score = evaluatePosition(fen);
    int expected = -20; // Isolated pawn penalty
    EXPECT_NEAR(score, expected, 20) << "White’s isolated pawn on D2 should give ~-20 penalty.";
}

// Test 7: Doubled pawns (White has doubled pawns on D-file)
TEST_F(EvaluationTest, PawnStructure_DoubledPawns) {
    std::string fen = "rnbqkbnr/pppppppp/8/8/3PP3/8/PPP2PPP/RNBQKBNR w KQkq - 0 1";
    int score = evaluatePosition(fen);
    int expected = -10; // Doubled pawn penalty
    EXPECT_NEAR(score, expected, 20) << "White’s doubled pawns on D-file should give ~-10 penalty.";
}

// Test 8: King safety (White has full pawn shield, Black has none)
TEST_F(EvaluationTest, KingSafety_PawnShield) {
    std::string fen = "rnbqkb1r/pppppppp/5n2/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    int score = evaluatePosition(fen);
    int expected = 30; // 3 pawns in White’s shield * 10, none for Black
    EXPECT_NEAR(score, expected, 20) << "White’s pawn shield should give ~30 centipawn advantage.";
}

// Test 9: Complex middlegame position
TEST_F(EvaluationTest, ComplexPosition) {
    std::string fen = "r1bqkb1r/pppp1ppp/5n2/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1";
    int score = evaluatePosition(fen);
    EXPECT_NEAR(score, 0, 50) << "Middlegame position should be roughly balanced (±50 centipawns).";
}

// Test 10: Endgame with passed pawn and king activity
TEST_F(EvaluationTest, Endgame_PassedPawnKingActivity) {
    std::string fen = "8/8/8/3P4/8/8/6K1/6k1 w - - 0 1";
    int score = evaluatePosition(fen);
    int expected = 100; // Passed pawn bonus + king activity (PST contribution)
    EXPECT_GT(score, expected)
        << "White’s passed pawn and active king should give a significant advantage.";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
