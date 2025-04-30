/**
 * @file
 * @brief
 */

#pragma once
#include "helper.hpp"
#include "moves.hpp"
#include "pieceList.hpp"
#include "state.hpp"

#include <cstdint>
#include <stack>
#include <vector>
typedef uint64_t board;

// Represents the current state of the board during a game.
// The state includes things such as: positions of all pieces, side to move,
// castling rights, en-passant square, etc. Some extra information is included
// as well to help with evaluation and move generation.

// The initial state of the board can be set from a FEN string, and moves are
// subsequently made (or undone) using the MakeMove and UnmakeMove functions.
/**
 * @class Board
 * @brief Board related functionality(including some bitboard stuff)
 *
 */
class Board {
  public:
    int whiteIndex = 0;
    int blackIndex = 1;

    // Stores piece code for each square on the board
    std::vector<int> square;

    // Square Index of black and white king
    std::array<int, 2> kingSquare;

    // Bitboards
    // Vector containing a bitboard For Each Piece type and color(i.e. black and
    // white)
    std::vector<board> piecesBitBoards;

    // Bitboards for all pieces of either colour (all white pieces, all black
    // pieces)
    std::vector<board> colourBitboards;
    board allPiecesBitboard;
    // Pieces by their movement abilities
    board FriendlyOrthogonalSliders;
    board FriendlyDiagonalSliders;
    board EnemyOrthogonalSliders;
    board EnemyDiagonalSliders;
    // Piece count excluding pawns and kings
    int totalPieceCountWithoutPawnsAndKings;
    // # Piece lists
    PieceList Bishops;
    PieceList Queens;
    PieceList Knights;
    PieceList Pawns;

    GameState currentGameState;
    int plyCount;
    int fiftyMoveCounter = currentGameState.fiftyMoveCounter;
    std::vector<Move> allGameMoves;

    // # Side to move info
    bool IsWhiteToMove;
    std::stack<uint64_t> repititionPositionHistory;
    void initialize();
    int getMoveColor() { return IsWhiteToMove ? Piece::White : Piece::Black; }
    int getOpponentColor() { return IsWhiteToMove ? Piece::Black : Piece::White; }
    int getOpponentMoveColorIndex() { return IsWhiteToMove ? whiteIndex : blackIndex; }
    int getMoveColorIndex() { return IsWhiteToMove ? whiteIndex : blackIndex; }

    void makeMove(Move move);

    void movePiece(piece_t movedPiece, int startSquare, int targetSquare);
    void updateSliderBitboards();
    void makeNullMove();
    void unmakeNullMove();

    // Is current player in check?
    // Note: caches check value so calling multiple times does not require recalculating
    bool isInCheck();
    bool calculateInCheckState();
    void loadStartPosition();
    void loadPosition(std::string fen);
    // void loadPosition(FenUtility.PositionInfo posInfo);
    static Board createBoard(std::string fen);
    static Board createBoard(Board source);

  private:
    std::vector<PieceList> allPieceLists;
};
