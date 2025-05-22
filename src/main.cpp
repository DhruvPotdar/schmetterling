
#include "board/board.hpp"
#include <iostream>
int main() {
    // Initialize board with standard chess position
    std::string initialFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Board board(initialFen);

    // Define a 10-move sequence (5 moves per side, Italian Opening with a capture)
    std::vector<std::pair<std::string, std::string>> moveList = {
        {"E2", "E4"}, // 1. e4
        {"E7", "E5"}, //    ... e5
        {"G1", "F3"}, // 2. Nf3
        {"B8", "C6"}, //    ... Nc6
        {"F1", "C4"}, // 3. Bc4
        {"G8", "F6"}, //    ... Nf6
        {"D2", "D4"}, // 4. d4
        {"E5", "D4"}, //    ... exd4 (capture)
        {"C2", "C3"}, // 5. c3
        {"F8", "C5"}  //    ... Bc5
    };

    std::cout << "Starting a 10-move chess game...\n";
    std::cout << "Initial position:\n";
    std::cout << board.createDiagram(board, true, true);

    // Execute each move and print the board
    for (size_t i = 0; i < moveList.size(); ++i) {
        const auto& [from, to] = moveList[i];
        Square fromSq(from);
        Square toSq(to);
        std::cout << "Move " << (i / 2 + 1) << (i % 2 == 0 ? ". " : "... ") << from << "-" << to
                  << "\n";

        // Make the move
        auto undoInfo = board.makeMove(fromSq, toSq);
        //
        // // Print the board after the move
        std::cout << Board::createDiagram(board, true, true);
        //
        // // Test unmaking the move (to verify state restoration)
        board.unMakeMove(fromSq, toSq, undoInfo);
        board.makeMove(fromSq, toSq); // Reapply the move to continue the game
    }

    std::cout << "Game completed!\n";
    return 0;
}
