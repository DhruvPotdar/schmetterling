#pragma once
#include <stdexcept>

class Side {
  private:
    int side;

  public:
    constexpr static int White = 0;
    constexpr static int Black = 1;
    Side() : side(White) {};

    Side(int _side) : side(_side) {
        if (_side != White && _side != Black) {
            throw std::invalid_argument("Invalid side value");
        }
    };

    operator int() const { return side; }

    Side operator!() const {
        if (side == White) return Black;
        return Side::White;
    }

    bool operator!=(const Side& other) const { return !(*this == other); }

    Side flip() const {
        if (side == White) return Black;
        return White;
    }
};

enum class PieceType { None = 0, Pawn = 1, Knight = 2, Bishop = 3, Rook = 4, Queen = 5, King = 6 };

struct Piece {
    Side side;
    PieceType type;
    int pieceIndex;

    Piece(char fenChar) {
        side = isupper(fenChar) ? Side::White : Side::Black;

        // Convert to lowercase for uniform processing
        char lc = tolower(fenChar);

        switch (lc) {
        case 'p':
            type = PieceType::Pawn;
            break;
        case 'n':
            type = PieceType::Knight;
            break;
        case 'b':
            type = PieceType::Bishop;
            break;
        case 'r':
            type = PieceType::Rook;
            break;
        case 'q':
            type = PieceType::Queen;
            break;
        case 'k':
            type = PieceType::King;
            break;
        default:
            throw std::invalid_argument("Invalid FEN piece character");
        }
    }
    Piece(PieceType pieceType, Side pieceColor)
        : side(pieceColor), type(pieceType),
          pieceIndex(static_cast<int>(pieceColor) * 6 + static_cast<int>(pieceType)) {}

    static bool isSide(Piece piece, Side color) { return (piece.side == color); }

    static bool isWhite(Piece piece) { return (piece.side == Side::White); }

    // Piece is bishop or queen
    static bool isDiagonalSlider(Piece piece) {
        return (piece.type == PieceType::Bishop) || (piece.type == PieceType::Queen);
    }

    // Piece is Rook or Queen
    static bool isOrthoSlider(Piece piece) {
        return (piece.type == PieceType::Rook) || (piece.type == PieceType::Queen);
    }
};
