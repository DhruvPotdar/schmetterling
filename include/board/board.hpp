/**
 * @file
 * @brief Information about the board
 */

#pragma once

#include "../moves/moves.hpp"
#include "bitboard.hpp"
#include "fen.hpp"

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

class FEN;

struct BoardState {
    std::array<BitBoard, 12> piecesBitBoards; // Indexed as Side * 6 + ((int) PieceType -1)
    std::array<BitBoard, 2> colorBitBoards;   // One for each color
    std::array<BitBoard, 2> diagonalSliders;  // Bishops + Queens (White, Black)
    std::array<BitBoard, 2> orthoSliders;     // Rooks + Queens (White, Black)
};

struct BoardHistory {
    BoardState state;
    Side side;
    int castlingRights;
    std::optional<Square> enPassantSquare;
    int halfMoveClock;
    int fullMoveClock;
    Piece capturedPiece;
    Move lastMove;
};

class Board {
  public:
    static const std::string startPositionFen;

    BoardState currentState; // Current Board

    Side side; /// Side to move, 0 - white, 1 - black

    uint8_t castlingRights; // Castling rights (bitmask: WhiteKingside | WhiteQueenside |
                            // BlackKingside | BlackQueenside);
    std::optional<Square> enPassantSquare;
    // Number of halfmoves wrt to the fiftyMove draw rule. It is reset at a pawn move or a capture
    // move and incremented otherwise
    int halfMoveClock;
    // Number of fullmoves in the game, starts at 1 and is incremented after blacks move
    int fullMoveClock;
    bool inCheckCache;

    // ANSI color codes for the diagram
    static constexpr std::string RESET = "\033[0m";
    static constexpr std::string BLACK_BG = "\033[1;48;5;66m";  // Dark teal square (#5f8787)
    static constexpr std::string WHITE_BG = "\033[1;48;5;151m"; // Light green square (#afffaf)
    static constexpr std::string BLACK_FG = "\033[1;38;5;232m"; // Very dark gray (#080808)
    static constexpr std::string WHITE_FG = "\033[1;38;5;231m"; // Bright white
    static constexpr std::string HIGHLIGHT_BG =
        "\033[1;48;5;220m"; // Bright yellow highlight (#ffd700)

    struct UndoInfo {
        Square from;
        Square to;
        Piece movedPiece;
        std::optional<Piece> capturedPiece;
        std::optional<PieceType> promotion;
        std::optional<Square> previousEnPassantSquare;
        uint8_t previousCastlingRights;
        int previousHalfmoveClock;
    };
    std::vector<UndoInfo> undoHistory;

    static constexpr int whiteKingside = 0b0001;
    static constexpr int whiteQueenside = 0b0010;
    static constexpr int blackKingside = 0b0100;
    static constexpr int blackQueenside = 0b1000;

    Board() {
        // Initialise Empty bitboard;
        currentState.colorBitBoards.fill(BitBoard());
        currentState.piecesBitBoards.fill(BitBoard());
        side = Side::White;
        castlingRights = whiteKingside | whiteQueenside | blackKingside | blackQueenside;
        enPassantSquare = std::nullopt;
        halfMoveClock = 0;
        fullMoveClock = 1;

        updateSliderBitboards();
    }

    Board(std::string fen) { FEN::parse(fen, *this); }
    std::string toFEN() const { return FEN::generate(*this); }

    constexpr operator std::string() { return Board::createDiagram(*this); }
    Piece getPieceAt(Square s) const;
    Piece getPieceAt(std::string squareName) const;

    UndoInfo makeMove(Square from, Square to);
    UndoInfo tryMove(Square from, Square to);
    void unMakeMove(Square from, Square to, const UndoInfo& undoInfo);
    void movePiece(Piece movedPiece, int startSquareIndex, int targetSquareIndex);
    void makeNullMove();
    void unmakeNullMove(const UndoInfo& undoInfo);
    /**
     * @brief Checks if the current player is in check
     * Note: caches check value so calling multiple
     * times does not require recalculating
     * @return  true if the current player is in check
     */
    bool isInCheck();
    bool calculateInCheckState() const;
    void updateSliderBitboards();

    static const std::string createDiagram(const Board& board, const bool blackAtTop = true,
                                           bool const includeFen = true);

    Board& operator=(const Board&) = default;
};
