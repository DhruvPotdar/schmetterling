#include "board/board.hpp"
#include "helpers/fen.hpp"
#include <iostream>

int main() {

    // Create a board with the starting position
    Board board = Board::createBoard(Fen::startPositionFen);

    std::cout << "Initial Board:\n" << static_cast<std::string>(board) << std::endl;

    // Make a few moves and print the board after each move
    Move e2e4(Board::squareIndexFromName("e2"), Board::squareIndexFromName("e4"));
    board.makeMove(e2e4, false);
    std::cout << "After e2-e4:\n" << static_cast<std::string>(board) << std::endl;

    Move g8f6(Board::squareIndexFromName("g8"), Board::squareIndexFromName("f6"));
    board.makeMove(g8f6, false);
    std::cout << "After ... Nf6:\n" << static_cast<std::string>(board) << std::endl;

    Move d2d4(Board::squareIndexFromName("d2"), Board::squareIndexFromName("d4"));
    board.makeMove(d2d4, false);
    std::cout << "After d2-d4:\n" << static_cast<std::string>(board) << std::endl;

    // Unmake the last move and print the board
    board.unmakeMove(d2d4, false);
    std::cout << "After undoing d2-d4:\n" << static_cast<std::string>(board) << std::endl;

    // Test loading a specific FEN string
    Board customBoard =
        Board::createBoard("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq c6 0 2");
    std::cout << "Custom Board (rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq c6 0 2):\n"
              << static_cast<std::string>(customBoard) << std::endl;

    // Test castling
    Board castlingBoard = Board::createBoard("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    std::cout << "Before White Kingside Castling:\n"
              << static_cast<std::string>(castlingBoard) << std::endl;
    Move whiteKingsideCastle(Board::squareIndexFromName("e1"), Board::squareIndexFromName("g1"),
                             MoveFlag::CastleFlag);
    castlingBoard.makeMove(whiteKingsideCastle, false);
    std::cout << "After White Kingside Castling:\n"
              << static_cast<std::string>(castlingBoard) << std::endl;
    castlingBoard.unmakeMove(whiteKingsideCastle, false);
    std::cout << "After undoing White Kingside Castling:\n"
              << static_cast<std::string>(castlingBoard) << std::endl;

    // Test pawn promotion
    Board promotionBoard = Board::createBoard("8/P7/8/8/8/8/8/8 w - - 0 1");
    std::cout << "Before White Pawn Promotion:\n"
              << static_cast<std::string>(promotionBoard) << std::endl;
    Move whitePromoteQueen(Board::squareIndexFromName("a7"), Board::squareIndexFromName("a8"),
                           MoveFlag::PromoteToQueenFlag);
    promotionBoard.makeMove(whitePromoteQueen, false);
    std::cout << "After White Pawn Promotion to Queen:\n"
              << static_cast<std::string>(promotionBoard) << std::endl;
    promotionBoard.unmakeMove(whitePromoteQueen, false);
    std::cout << "After undoing White Pawn Promotion:\n"
              << static_cast<std::string>(promotionBoard) << std::endl;

    // Test en passant
    Board enPassantBoard = Board::createBoard("8/8/8/p7/P7/8/8/8 w - a6 0 1");
    std::cout << "Before En Passant Capture:\n" << static_cast<std::string>(enPassantBoard) << std::endl;
    Move whiteEnPassant(Board::squareIndexFromName("a5"), Board::squareIndexFromName("b6"),
                        MoveFlag::EnPassantCaptureFlag);
    enPassantBoard.makeMove(whiteEnPassant, false);
    std::cout << "After En Passant Capture:\n" << static_cast<std::string>(enPassantBoard) << std::endl;
    enPassantBoard.unmakeMove(whiteEnPassant, false);
    std::cout << "After undoing En Passant Capture:\n"
              << static_cast<std::string>(enPassantBoard) << std::endl;

    // Test isInCheck function
    Board checkBoard1 = Board::createBoard("4k3/8/8/8/4r3/8/8/4K3 w - - 0 1");
    std::cout << "Board in check (White)? " << checkBoard1.isInCheck() << std::endl;

    Board checkBoard2 = Board::createBoard("4k3/8/8/8/4r3/8/8/4K3 b - - 0 1");
    std::cout << "Board in check (Black)? " << checkBoard2.isInCheck() << std::endl;

    return 0;
}
