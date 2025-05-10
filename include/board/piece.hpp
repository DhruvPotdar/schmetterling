/**
 * @file
 * @brief Contains definitions for each piece type (represented as integers), as well as various
 * helper functions for dealing with pieces.static class
 */

#ifndef PIECE_HPP

#define PIECE_HPP

#include <string>
#include <vector>

typedef int piece_t;
class Piece {
  public:
    // Piece Class
    static const piece_t None = 0;
    static const piece_t Pawn = 1;
    static const piece_t Knight = 2;
    static const piece_t Bishop = 3;
    static const piece_t Rook = 4;
    static const piece_t Queen = 5;
    static const piece_t King = 6;

    // Piece Colours
    static const int White = 0;
    static const int Black = 8;

    // Pieces
    static const piece_t WhitePawn = Pawn | White;     // 1
    static const piece_t WhiteKnight = Knight | White; // 2
    static const piece_t WhiteBishop = Bishop | White; // 3
    static const piece_t WhiteRook = Rook | White;     // 4
    static const piece_t WhiteQueen = Queen | White;   // 5
    static const piece_t WhiteKing = King | White;     // 6

    static const piece_t BlackPawn = Pawn | Black;     // 9
    static const piece_t BlackKnight = Knight | Black; // 10
    static const piece_t BlackBishop = Bishop | Black; // 11
    static const piece_t BlackRook = Rook | Black;     // 12
    static const piece_t BlackQueen = Queen | Black;   // 13
    static const piece_t BlackKing = King | Black;     // 14

    static const int MaxPieceIndex = BlackKing;
    std::vector<int> PieceIndices = {WhitePawn,   WhiteKnight, WhiteBishop, WhiteRook,
                                     WhiteQueen,  WhiteKing,   BlackPawn,   BlackKnight,
                                     BlackBishop, BlackRook,   BlackQueen,  BlackKing};

    // Bit Masks
    static const int typeMask = 0b0111;
    static const int colorMask = 0b1000;

    static piece_t makePiece(piece_t piece_class, int piece_colors) {
        return piece_class | piece_colors;
    }

    static piece_t makePiece(piece_t piece_class, bool pieceIsWhite) {
        return makePiece(piece_class, pieceIsWhite ? White : Black);
    }

    // Returns true if given piece matches the given colour. If piece is of type
    // 'none', result will always be false.
    static bool isColor(piece_t piece, int color) {
        return (piece & colorMask) == color && piece != 0;
    }

    static bool isWhite(piece_t piece) { return isColor(piece, White); }

    static int pieceColor(piece_t piece) { return piece & colorMask; }
    static int pieceClass(piece_t piece) { return piece & typeMask; }

    // Bishop or Queen
    static bool isDiagonalSlider(piece_t piece) {
        auto piece_class = pieceClass(piece);
        return (piece_class == Bishop) || (piece_class == Queen);
    }

    // Piece class is Rook or Queen
    static bool isOrthoSlider(piece_t piece) {
        auto piece_class = pieceClass(piece);
        return (piece_class == Queen) || (piece_class == Rook);
    }

    // Bishop rook or queen
    static bool isSlider(piece_t piece) {
        auto piece_class = pieceClass(piece);
        return (piece_class == Bishop) || (piece_class == Queen) || (piece_class == Queen);
    }
    static char getPieceCharacter(piece_t piece) {
        int piece_class = pieceClass(piece);
        char pieceCharacter;
        switch (piece_class) {
        case Rook:
            pieceCharacter = 'R';
        case Knight:
            pieceCharacter = 'N';
        case Bishop:
            pieceCharacter = 'B';
        case Queen:
            pieceCharacter = 'Q';
        case King:
            pieceCharacter = 'K';
        case Pawn:
            pieceCharacter = 'P';
        default:
            pieceCharacter = ' ';
        };
        pieceCharacter = isWhite(piece) ? pieceCharacter : std::tolower(pieceCharacter);
        return pieceCharacter;
    }

    static std::string getPieceSymbol(piece_t piece) {
        int piece_class = pieceClass(piece);
        bool is_white = isWhite(piece);
        std::string pieceSymbol;

        switch (piece_class) {
        case Rook:
            pieceSymbol = is_white ? "♖" : "♜";
            break;
        case Knight:
            pieceSymbol = is_white ? "♘" : "♞";
            break;
        case Bishop:
            pieceSymbol = is_white ? "♗" : "♝";
            break;
        case Queen:
            pieceSymbol = is_white ? "♕" : "♛";
            break;
        case King:
            pieceSymbol = is_white ? "♔" : "♚";
            break;
        case Pawn:
            pieceSymbol = is_white ? "♙" : "♟";
            break;
        default:
            pieceSymbol = ' ';
        };
        return pieceSymbol;
    }

    static int GetPieceTypeFromClass(char piece_class) {
        piece_class = std::toupper(piece_class);

        switch (piece_class) {
        case 'R':
            return Rook;
        case 'N':
            return Knight;
        case 'B':
            return Bishop;
        case 'Q':
            return Queen;
        case 'K':
            return King;
        case 'P':
            return Pawn;
        default:
            return ' ';
        }
    }
};
#endif // !PIECE_HPP
