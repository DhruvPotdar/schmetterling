#pragma once

#include "board/board.hpp"

class BitBoardUtil {

  public:
    BitBoardUtil() {
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                processSquare(x, y);
            }
        }
    }
    static constexpr board fileA = 0x101010101010101;
    static constexpr auto notAFile = ~fileA;
    static constexpr auto notHFile = ~(fileA << 7);

    static constexpr board rank1 = 0b11111111;
    static constexpr board rank2 = rank1 << 8;
    static constexpr board rank3 = rank2 << 8;
    static constexpr board rank4 = rank3 << 8;
    static constexpr board rank5 = rank4 << 8;
    static constexpr board rank6 = rank5 << 8;
    static constexpr board rank7 = rank6 << 8;
    static constexpr board rank8 = rank7 << 8;

    static std::array<board, 64> knightAttacks;
    static std::array<board, 64> kingMoves;
    static std::array<board, 64> whitePawnAttacks;
    static std::array<board, 64> blackPawnAttacks;

    // Orthogonal directions: left, up, right, down
    std::array<Coord, 4> orthoDir = {
        Coord(-1, 0), // Left
        Coord(0, 1),  // Up
        Coord(1, 0),  // Right
        Coord(0, -1)  // Down
    };

    // Diagonal directions: bottom-left, top-left, top-right, bottom-right
    std::array<Coord, 4> diagDir = {
        Coord(-1, -1), // Bottom-left
        Coord(-1, 1),  // Top-left
        Coord(1, 1),   // Top-right
        Coord(1, -1)   // Bottom-right
    };

    // Knight jumps: all eight possible L-shaped moves
    std::array<Coord, 8> knightJumps = {Coord(-2, -1), Coord(-2, 1), Coord(-1, 2), Coord(1, 2),
                                        Coord(2, 1),   Coord(2, -1), Coord(1, -2), Coord(-1, -2)};

    static int popLSB(board bitBoard) {
        if (bitBoard == 0) return -1; // Empty BitBoard
        const auto index = std::countr_zero(bitBoard);
        bitBoard &= bitBoard - 1; // Clear the least significant bit
        return index;
    }

    static void setSquare(board& bitBoard, int squareIndex) { bitBoard |= 1ULL << squareIndex; }

    static void clearSquare(board& bitBoard, int squareIndex) {
        bitBoard &= ~(1ULL << squareIndex);
    }

    static void toggleSquare(board& bitBoard, int squareIndex) {
        bitBoard ^= (1ULL << squareIndex);
    }

    static void toggleSquares(board& bitBoard, int squareA, int squareB) {
        bitBoard ^= (1ULL << squareA | 1ULL << squareB);
    }

    static bool containsSquare(board& bitBoard, int square) {
        return ((bitBoard >> square) & 1) != 0;
    }

    static board shift(board bitBoard, int numSquaresToShift) {
        if (numSquaresToShift > 0) {
            return bitBoard << numSquaresToShift;
        } else {
            return bitBoard >> -numSquaresToShift;
        }
    }

    /**
     * @brief: Pawn attacks are calculated like so: (example given with white to move)
     * The first half of the attacks are calculated by shifting all pawns north-east:
     * northEastAttacks = pawnBitboard << 9 Note that pawns on the h file will be wrapped around to
     * the a file, so then mask out the a file: northEastAttacks &= notAFile (Any pawns that were
     * originally on the a file will have been shifted to the b file, so a file should be empty).
     * The other half of the attacks are calculated by shifting all pawns north-west. This time the
     * h file must be masked out. Combine the two halves to get a bitboard with all the pawn
     * attacks: northEastAttacks | northWestAttacks
     *
     * @param pawnBitBoard
     * @param isWhite
     * @return
     */
    static board pawnAttacks(board pawnBitBoard, bool isWhite) {
        if (isWhite) {
            return ((pawnBitBoard << 9) & notAFile) | ((pawnBitBoard >> 7) & notHFile);
        }
        return ((pawnBitBoard >> 7) & notAFile) | ((pawnBitBoard >> 9) & notHFile);
    }

  private:
    void processSquare(int x, int y) {

        auto validSquareIndex = [](int x, int y, int& index) -> bool {
            index = y * 8 + x;
            return x >= 0 && x < 8 && y >= 0 && y < 8;
        };

        const auto squareIndex = y * 8 + x;
        Coord current(x, y);

        for (int dirIndex = 0; dirIndex < 4; ++dirIndex) {
            for (auto dst = 1; dst < 8; dst++) {
                const auto orthoX = x + orthoDir[dirIndex].x * dst;
                const auto orthoY = y + orthoDir[dirIndex].y * dst;
                const auto diagX = x + diagDir[dirIndex].x * dst;
                const auto diagY = y + diagDir[dirIndex].y * dst;

                int orthoTargetIndex;
                if (validSquareIndex(orthoX, orthoY, orthoTargetIndex)) {
                    if (dst == 1) {
                        kingMoves[squareIndex] |= 1ULL << orthoTargetIndex;
                    }
                }

                int diagTargetIndex;
                if (validSquareIndex(diagX, diagY, diagTargetIndex)) {
                    if (dst == 1) {
                        if (dst == 1) {
                            kingMoves[squareIndex] |= 1ULL << diagTargetIndex;
                        }
                    }
                }
            }
        }

        // Knight jumps
        for (auto jump : knightJumps) {
            const auto knightX = x + jump.x;
            const auto knightY = y + jump.y;

            int knightTargetSquare;
            if (validSquareIndex(knightX, knightY, knightTargetSquare)) {
                knightAttacks[squareIndex] |= 1ULL << knightTargetSquare;
            }
        }

        // Pawn attacks
        int whitePawnRight;
        if (validSquareIndex(x + 1, y + 1, whitePawnRight)) {
            whitePawnAttacks[squareIndex] |= 1ul << whitePawnRight;
        }
        int whitePawnLeft;
        if (validSquareIndex(x - 1, y + 1, whitePawnLeft)) {
            whitePawnAttacks[squareIndex] |= 1ul << whitePawnLeft;
        }
        int blackPawnAttackRight;
        if (validSquareIndex(x + 1, y - 1, blackPawnAttackRight)) {
            blackPawnAttacks[squareIndex] |= 1ul << blackPawnAttackRight;
        }
        int blackPawnAttackLeft;
        if (validSquareIndex(x - 1, y - 1, blackPawnAttackLeft)) {
            blackPawnAttacks[squareIndex] |= 1ul << blackPawnAttackLeft;
        }
    }
};
