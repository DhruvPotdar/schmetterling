
#pragma once
#include <array>

class PieceList
{
  private:
    // Indices of squares occupied by given piece type
    std::array<int, 16> occupiedSquares;
    // Map from square index to index in occupiedSquares array
    std::array<int, 64> map;
    int numPieces;

  public:
    PieceList(int maxPieceCount = 16) : numPieces(0)
    {
        occupiedSquares.fill(0);
        map.fill(0);
    }

    int Count() const { return numPieces; }

    void AddPieceAtSquare(int square)
    {
        occupiedSquares[numPieces] = square;
        map[square] = numPieces;
        numPieces++;
    }

    void RemovePieceAtSquare(int square)
    {
        int pieceIndex = map[square];
        occupiedSquares[pieceIndex] = occupiedSquares[numPieces - 1];
        map[occupiedSquares[pieceIndex]] = pieceIndex;
        numPieces--;
    }

    void MovePiece(int startSquare, int targetSquare)
    {

        int pieceIndex = map[startSquare];
        occupiedSquares[pieceIndex] = targetSquare;
        map[targetSquare] = pieceIndex;
    }

    int operator[](int index) const { return occupiedSquares[index]; }
};
