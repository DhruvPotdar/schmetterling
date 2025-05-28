#pragma once

#include "board/types.hpp"
#include <array>

/**
 * @class PieceSquareTables
 * @brief Piece Square tables for evaluation. Tables in the code are for white.
 * - Use the get function to get the right table based on the piece you have
 */
class PieceSquareTables {
  public:
    // Get the appropriate piece-square table for a given piece
    static const std::array<int, 64> get(Piece piece) {
        std::array<int, 64> table;

        switch (piece.type) {
        case PieceType::Pawn:
            table = pawn;
            break;
        case PieceType::Knight:
            table = knight;
            break;
        case PieceType::Bishop:
            table = bishop;
            break;
        case PieceType::Rook:
            table = rook;
            break;
        case PieceType::Queen:
            table = queen;
            break;
        case PieceType::King:
            // Default to middle game king table for now
            table = kingMiddle;
            break;
        case PieceType::None:
        default:
            throw std::invalid_argument("Invalid piece type for piece-square table");
        }

        // If the piece is black, flip the table vertically
        if (piece.side == Side::Black) {
            return flip(table);
        }

        return table;
    }

    // Get king table specifically for endgame
    static const std::array<int, 64> getKingEndgame(Side side) {
        if (side == Side::Black) {
            return flip(kingEndTable);
        }
        return kingEndTable;
    }

    // Get king table specifically for middle game
    static const std::array<int, 64> getKingMiddlegame(Side side) {
        if (side == Side::Black) {
            return flip(kingMiddle);
        }
        return kingMiddle;
    }

    // Flip a table vertically (for black pieces)
    static const std::array<int, 64> flip(std::array<int, 64> table) {
        std::array<int, 64> flippedTable;

        // Flip the table by swapping rows
        // Row 0 becomes row 7, row 1 becomes row 6, etc.
        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                int originalIndex = rank * 8 + file;
                int flippedIndex = (7 - rank) * 8 + file;
                flippedTable[flippedIndex] = table[originalIndex];
            }
        }

        return flippedTable;
    }

  private:
    // clang-format off
    static constexpr std::array<int, 64> pawn = { 
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    static constexpr std::array<int, 64> knight = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50,
    };

    static constexpr std::array<int, 64> bishop = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20,
    };

    static constexpr std::array<int, 64> rook = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  5,  5,  0,  0,  0
    };

    static constexpr std::array<int, 64> queen = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  5,  5,  5,  0, -5,
        0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };

    static constexpr std::array<int, 64> kingMiddle = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        20, 20,  0,  0,  0,  0, 20, 20,
        20, 30, 10,  0,  0, 10, 30, 20
    };

    static constexpr std::array<int, 64> kingEndTable = {
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
    };

    // clang-format on
};
