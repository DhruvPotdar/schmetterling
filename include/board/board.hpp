/**
 * @file
 * @brief: Represents the current state of the board during a game. The state includes things such
 * as: positions of all pieces, side to move, castling rights, en-passant square, etc. Some extra
 * information is included as well to help with evaluation and move generation.
 */

#pragma once

#include "board/coord.hpp"
#include "board/pieceList.hpp"

#include "helpers/fen.hpp"
#include "moves.hpp"
#include "state.hpp"

#include <cstdint>
#include <stack>
#include <vector>

class Fen;
// struct PositionInfo;

typedef uint64_t board;

/**
 * @class Board
 * @brief Board related functionality(including some bitboard stuff)
 * The initial state of the board can be set from a Fen string, and moves are
 * subsequently made (or undone) using the MakeMove and UnmakeMove functions.
 */
class Board {
  public:
    static const auto whiteIndex = 0;
    static const auto blackIndex = 1;

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
    std::vector<board> colorBitboards;
    board allPiecesBitboard;

    // Pieces by their movement abilities
    board friendlyOrthogonalSliders;
    board friendlyDiagonalSliders;
    board enemyOrthogonalSliders;
    board enemyDiagonalSliders;

    // Piece count excluding pawns and kings
    int totalPieceCountWithoutPawnsAndKings;

    // # Piece lists
    // For per-color lists
    std::array<PieceList, 2> Bishops;
    std::array<PieceList, 2> Queens;
    std::array<PieceList, 2> Knights;
    std::array<PieceList, 2> Pawns;
    std::array<PieceList, 2> Rooks;

    GameState currentGameState;
    // Ply is a half move
    int plyCount;

    std::vector<Move> allGameMoves;

    // # Side to move info
    bool isWhiteToMove;
    std::stack<uint64_t> repititionPositionHistory;

    void initialize();
    int getFiftyMoveCounter() { return currentGameState.fiftyMoveCounter; }
    std::string getCurrentFen();

    int getMoveColor() { return isWhiteToMove ? Piece::White : Piece::Black; }
    int getOpponentColor() { return isWhiteToMove ? Piece::Black : Piece::White; }
    int getOpponentMoveColorIndex() { return isWhiteToMove ? whiteIndex : blackIndex; }
    int getMoveColorIndex() { return isWhiteToMove ? whiteIndex : blackIndex; }

    void makeMove(Move move, bool inSearch);
    void unmakeMove(Move move, bool inSearch);

    void movePiece(piece_t movedPiece, int startSquare, int targetSquare);
    void updateSliderBitboards();
    void makeNullMove();
    void unmakeNullMove();

    /**
     * @brief Checks if the current player is in check
     * Note: caches check value so calling multiple
     * times does not require recalculating
     * @return  true if the current player is in check
     */
    bool isInCheck();
    bool calculateInCheckState();
    void loadStartPosition();
    void loadPosition(Fen::PositionInfo* posInfo);
    void loadPosition(std::string fen);
    static Board createBoard(std::string fen);
    static Board createBoard(Board source);

    operator std::string() const {
        // return Board::createDiagram(*this, !isWhiteToMove, true);

        return Board::createDiagram(*this, true, true);
    }

    static const std::array<Coord, 4> rookDirections;
    static const std::array<Coord, 4> bishopDirections;

    constexpr static const std::string fileNames = "abcdefgh";
    constexpr static const std::string rankNames = "12345678";

    static const int a1 = 0;
    static const int b1 = 1;
    static const int c1 = 2;
    static const int d1 = 3;
    static const int e1 = 4;
    static const int f1 = 5;
    static const int g1 = 6;
    static const int h1 = 7;

    static const int a8 = 56;
    static const int b8 = 57;
    static const int c8 = 58;
    static const int d8 = 59;
    static const int e8 = 60;
    static const int f8 = 61;
    static const int g8 = 62;
    static const int h8 = 63;

    // Rank (0 to 7) of square
    static int rankIndex(int squareIndex) { return squareIndex >> 3; }

    // File (0 to 7) of square
    static int fileIndex(int squareIndex) { return squareIndex & 0b000111; }

    static int indexFromCoord(int fileIndex, int rankIndex) { return rankIndex * 8 + fileIndex; }

    static int indexFromCoord(const Coord& coord) { return indexFromCoord(coord.x, coord.y); }

    static Coord coordFromIndex(int squareIndex) {
        return Coord(fileIndex(squareIndex), rankIndex(squareIndex));
    }

    static bool lightSquare(int fileIndex, int rankIndex) {
        return (fileIndex + rankIndex) % 2 != 0;
    }

    static bool LightSquare(int squareIndex) {
        return lightSquare(fileIndex(squareIndex), rankIndex(squareIndex));
    }

    static std::string squareNameFromCoordinate(int fileIndex, int rankIndex) {
        return std::string(1, fileNames[fileIndex]) + std::to_string(rankIndex + 1);
    }

    static std::string squareNameFromIndex(int squareIndex) {
        return squareNameFromCoordinate(coordFromIndex(squareIndex));
    }

    static std::string squareNameFromCoordinate(const Coord& coord) {
        return squareNameFromCoordinate(coord.x, coord.y);
    }

    static int squareIndexFromName(const std::string& name) {
        char fileName = name[0];
        char rankName = name[1];
        int fileIndex = fileNames.find(fileName);
        int rankIndex = rankNames.find(rankName);
        return indexFromCoord(fileIndex, rankIndex);
    }

    static std::string createDiagram(const Board& board, bool blackAtTop, bool includeFen);

    static bool isValidCoordinate(int x, int y) { return x >= 0 && x < 8 && y >= 0 && y < 8; }

  private:
    std::array<PieceList, Piece::MaxPieceIndex + 1> allPieceLists;
    std::vector<GameState> gameStateHistory;
    bool cachedInCheckValue;
    bool hasCachedInCheckValue;
    Fen::PositionInfo* startPositionInfo;
};
