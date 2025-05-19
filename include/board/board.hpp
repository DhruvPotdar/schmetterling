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
    std::array<BitBoard, 12> piecesBitBoards; // Indexed as Side * 6 + (int) PieceType
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
    // Current Board
    BoardState currentState;

    /// Side to move, 0 - white, 1 - black
    Side side;
    // Castling rights (bitmask: WhiteKingside | WhiteQueenside | BlackKingside | BlackQueenside)
    uint8_t castlingRights;
    std::optional<Square> enPassantSquare;
    // Number of halfmoves wrt to the fiftyMove draw rule. It is reset at a pawn move or a capture move
    // and incremented otherwise
    int halfMoveClock;
    // Number of fullmoves in the game, starts at 1 and is incremented after blacks move
    int fullMoveClock;
    bool inCheckCache;

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
    std::vector<UndoInfo> moveHistory;

    static constexpr int whiteKingside = 0b0001;
    static constexpr int whiteQueenside = 0b0010;
    static constexpr int blackKingside = 0b0100;
    static constexpr int blackQueenside = 0b1000;

    Board() {
        // Initialise Empty bitboard;
        currentState.colorBitBoards.fill(BitBoard());
        currentState.piecesBitBoards.fill(0);
        side = Side::White;
        castlingRights = whiteKingside | whiteQueenside | blackKingside | blackQueenside;
        enPassantSquare = std::nullopt;
        halfMoveClock = 0;
        fullMoveClock = 1;

        updateSliderBitboards();
    }

    Board(std::string fen) { FEN::parse(fen, *this); }
    std::string toFEN() const { return FEN::generate(*this); }

    Piece getPieceAt(Square s) const;

    UndoInfo makeMove(Square from, Square to);
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

    // ANSI color codes for the diagram
    static constexpr std::string RESET = "\033[0m";
    static constexpr std::string BLACK_BG = "\033[40m";
    static constexpr std::string WHITE_BG = "\033[47m";
    static constexpr std::string BLACK_FG = "\033[30m";
    static constexpr std::string WHITE_FG = "\033[37m";
    static constexpr std::string HIGHLIGHT_BG = "\033[43m";
    operator std::string() { return Board::createDiagram(*this, true, true); }

    const std::string createDiagram(const Board& board, const bool blackAtTop = true,
                                    bool const includeFen = true);

    Board& operator=(const Board&) = default;
};
