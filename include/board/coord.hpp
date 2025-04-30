#pragma once

// #include "BoardHelper.h"

// Class for representing squares on the chess board as file/rank integer pairs.
// (0, 0) = a1, (7, 7) = h8. Coords can also be used as offsets. For example,
// while a Coord of (-1, 0) is not a valid square, it can be used to represent
// the concept of moving 1 square left.

class Coord {
  public:
    const int fileIndex;
    const int rankIndex;

    Coord(int fileIndex, int rankIndex) : fileIndex(fileIndex), rankIndex(rankIndex) {}

    // Coord(int squareIndex)
    //     :
    //     fileIndex(BoardHelper::FileIndex(squareIndex)),
    //       rankIndex(BoardHelper::RankIndex(squareIndex))
    // {
    // }

    bool IsLightSquare() const { return (fileIndex + rankIndex) % 2 != 0; }

    int CompareTo(const Coord& other) const {
        return (fileIndex == other.fileIndex && rankIndex == other.rankIndex) ? 0 : 1;
    }

    Coord operator+(const Coord& other) const {
        return Coord(fileIndex + other.fileIndex, rankIndex + other.rankIndex);
    }

    Coord operator-(const Coord& other) const {
        return Coord(fileIndex - other.fileIndex, rankIndex - other.rankIndex);
    }

    Coord operator*(int m) const { return Coord(fileIndex * m, rankIndex * m); }

    friend Coord operator*(int m, const Coord& a) { return a * m; }

    bool IsValidSquare() const {
        return fileIndex >= 0 && fileIndex < 8 && rankIndex >= 0 && rankIndex < 8;
    }

    // int SquareIndex() const { return
    // BoardHelper::IndexFromCoord(*this); }
};
