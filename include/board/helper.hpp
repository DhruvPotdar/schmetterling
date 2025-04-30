#ifndef CHESS_CORE_BOARDHELPER_HPP
#define CHESS_CORE_BOARDHELPER_HPP

#include <array>
#include <string>

#include "coord.hpp"

class Board;      // Assumed to have Square array, AllGameMoves vector, ZobristKey, etc.
class Piece;      // Assumed to have static GetSymbol method
class FenUtility; // Assumed to have static CurrentFen method

class BoardHelper {
  public:
    static const std::array<Coord, 4> RookDirections;
    static const std::array<Coord, 4> BishopDirections;

    static const std::string fileNames;
    static const std::string rankNames;

    static constexpr int a1 = 0;
    static constexpr int b1 = 1;
    static constexpr int c1 = 2;
    static constexpr int d1 = 3;
    static constexpr int e1 = 4;
    static constexpr int f1 = 5;
    static constexpr int g1 = 6;
    static constexpr int h1 = 7;

    static constexpr int a8 = 56;
    static constexpr int b8 = 57;
    static constexpr int c8 = 58;
    static constexpr int d8 = 59;
    static constexpr int e8 = 60;
    static constexpr int f8 = 61;
    static constexpr int g8 = 62;
    static constexpr int h8 = 63;

    // Rank (0 to 7) of square
    static int RankIndex(int squareIndex) { return squareIndex >> 3; }

    // File (0 to 7) of square
    static int FileIndex(int squareIndex) { return squareIndex & 0b000111; }

    static int IndexFromCoord(int fileIndex, int rankIndex) { return rankIndex * 8 + fileIndex; }

    static int IndexFromCoord(const Coord& coord) {
        return IndexFromCoord(coord.fileIndex, coord.rankIndex);
    }

    static Coord CoordFromIndex(int squareIndex) {
        return Coord(FileIndex(squareIndex), RankIndex(squareIndex));
    }

    static bool LightSquare(int fileIndex, int rankIndex) {
        return (fileIndex + rankIndex) % 2 != 0;
    }

    static bool LightSquare(int squareIndex) {
        return LightSquare(FileIndex(squareIndex), RankIndex(squareIndex));
    }

    static std::string SquareNameFromCoordinate(int fileIndex, int rankIndex) {
        return std::string(1, fileNames[fileIndex]) + std::to_string(rankIndex + 1);
    }

    static std::string SquareNameFromIndex(int squareIndex) {
        return SquareNameFromCoordinate(CoordFromIndex(squareIndex));
    }

    static std::string SquareNameFromCoordinate(const Coord& coord) {
        return SquareNameFromCoordinate(coord.fileIndex, coord.rankIndex);
    }

    static int SquareIndexFromName(const std::string& name) {
        char fileName = name[0];
        char rankName = name[1];
        int fileIndex = fileNames.find(fileName);
        int rankIndex = rankNames.find(rankName);
        return IndexFromCoord(fileIndex, rankIndex);
    }

    static bool IsValidCoordinate(int x, int y) { return x >= 0 && x < 8 && y >= 0 && y < 8; }
};

// Static member definitions
inline const std::array<Coord, 4> BoardHelper::RookDirections = {Coord(-1, 0), Coord(1, 0),
                                                                 Coord(0, 1), Coord(0, -1)};

inline const std::array<Coord, 4> BoardHelper::BishopDirections = {Coord(-1, 1), Coord(1, 1),
                                                                   Coord(1, -1), Coord(-1, -1)};

inline const std::string BoardHelper::fileNames = "abcdefgh";
inline const std::string BoardHelper::rankNames = "12345678";

#endif
