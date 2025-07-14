#include "board/board.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    // Check for required depth argument
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <depth> [fen]\n";
        std::cerr << "Example: " << argv[0] << " 5\n";
        std::cerr
            << "Example: " << argv[0]
            << " 3 \"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/5N2/PPPPQPPP/RNB1KB1R w KQkq - 0 1\"\n";
        return 1;
    }

    const auto depth = std::atoi(argv[1]);
    if (depth <= 0) {
        std::cerr << "Error: Depth must be a positive integer\n";
        return 1;
    }

    // Default FEN: Starting position
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string initialFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string kiwipete = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    std::string pos3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
    std::string pos4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    std::string pos5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    if (argc > 2) {
        fen = argv[2];
    }

    try {
        Board board(fen);
        std::cout << "Running perft test for FEN: " << fen << " at depth " << depth << "\n";
        std::cout << Board::createDiagram(board);
        board.perft(depth, true); // Run perft with verbose output
        // board.perftDivide(depth); // Run perftDivide
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid FEN or board setup: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
