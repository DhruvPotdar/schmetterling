#pragma once
#include <iostream>
#include <stdexcept>

class Side {
  private:
    bool side;

  public:
    constexpr static bool White = 0;
    constexpr static bool Black = 1;
    Side() : side(White) {};

    Side(bool _side) : side(_side) {
        if (_side != White && _side != Black) {
            throw std::invalid_argument("Invalid side value");
        }
    };

    operator bool() const { return side; }

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

enum class PieceType { Pawn = 0, Knight = 1, Bishop = 2, Rook = 3, Queen = 4, King = 5, None = 6 };

struct Piece {
    Side side;
    PieceType type;

    int pieceIndex() const {
        if (type == PieceType::None) {
            throw std::logic_error("Cannot compute pieceIndex for PieceType::None");
        }
        return static_cast<int>(side) * 6 + (static_cast<int>(type));
    }

    Piece(char fenChar) {
        side = isupper(fenChar) ? Side::White : Side::Black;

        // Convert to lowercase for uniform processing
        const auto lc = tolower(fenChar);

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

    Piece(const PieceType pieceType, const Side pieceColor) {
        if (pieceType <= PieceType::None && pieceColor <= 1) {
            side = pieceColor;
            type = pieceType;
        } else {
            throw std::invalid_argument("WARNING WRONG Piece Declared");
        }
    }

    Piece(int pieceIndex) {}

    static bool isSide(const Piece piece, const Side color) { return (piece.side == color); }

    static bool isWhite(const Piece piece) { return (piece.side == Side::White); }

    // Piece is bishop or queen
    static bool isDiagonalSlider(const Piece piece) {
        return (piece.type == PieceType::Bishop) || (piece.type == PieceType::Queen);
    }

    // Piece is Rook or Queen
    static bool isOrthoSlider(const Piece piece) {
        return (piece.type == PieceType::Rook) || (piece.type == PieceType::Queen);
    }

    std::string getPieceSymbol() {
        std::string pieceSymbol;
        switch (type) {
        case PieceType::Rook:
            pieceSymbol = "󰡛 ";
            break;
        case PieceType::Knight:
            pieceSymbol = "󰡘 ";
            break;
        case PieceType::Bishop:
            pieceSymbol = "󰡜 ";
            break;
        case PieceType::Queen:
            pieceSymbol = "󰡚 ";
            break;
        case PieceType::King:
            pieceSymbol = "󰡗 ";
            break;
        case PieceType::Pawn:
            pieceSymbol = "󰡙 ";
            break;
        default:
            pieceSymbol = ' ';
        };
        return pieceSymbol;
    }
};
