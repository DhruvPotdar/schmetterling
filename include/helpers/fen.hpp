#pragma once

#include "board/piece.hpp"
#include <span>
#include <sstream>
#include <string>
#include <vector>

class Board;

class Fen {
  public:
    inline const static std::string startPositionFen =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    struct PositionInfo {
      private:
        std::vector<int> internalSquares;

      public:
        std::string fen;
        std::span<const piece_t> getSquares() const { return internalSquares; }

        bool whiteCastleKingside;
        bool whiteCastleQueenside;
        bool blackCastleKingside;
        bool blackCastleQueenside;
        // En passant file (1 is a-file, 8 is h-file, 0 means none)
        int enPassantFile;
        bool whiteToMove;
        // Number of half-moves since last capture or pawn advance
        // (starts at 0 and increments after each player's move)
        int fiftyMovePlyCount;
        // Total number of moves played in the game
        // (starts at 1 and increments after black's move)
        int moveCount;

        PositionInfo(std::string fen) {
            std::vector<int> square(64, Piece::None);
            const auto sections = split(fen);

            auto file = 0;
            auto rank = 7;

            // sections[0] is the first part of the fen (piece placement)
            for (char symbol : sections[0]) {
                if (symbol == '/') {
                    file = 0;
                    rank--;
                } else if (std::isdigit(symbol)) {
                    file += symbol - '0'; // Convert char digit to int
                } else {

                    int pieceColor = std::isupper(symbol) ? Piece::White : Piece::Black;

                    char lower = std::tolower(symbol);
                    auto pieceClass = Piece::None;
                    switch (lower) {
                    case 'k':
                        pieceClass = Piece::King;
                        break;
                    case 'p':
                        pieceClass = Piece::Pawn;
                        break;
                    case 'n':
                        pieceClass = Piece::Knight;
                        break;
                    case 'b':
                        pieceClass = Piece::Bishop;
                        break;
                    case 'r':
                        pieceClass = Piece::Rook;
                        break;
                    case 'q':
                        pieceClass = Piece::Queen;
                        break;
                    }

                    square[rank * 8 + file] = pieceClass | pieceColor;
                    file++;
                }
            }
            std::string castlingRights = sections[2];
            whiteCastleKingside = castlingRights.find('K') != std::string::npos;
            whiteCastleQueenside = castlingRights.find('Q') != std::string::npos;
            blackCastleKingside = castlingRights.find('k') != std::string::npos;
            blackCastleQueenside = castlingRights.find('q') != std::string::npos;
            whiteToMove = (sections[1] == "w");
            enPassantFile = 0;
            fiftyMovePlyCount = 0;
            moveCount = 0;

            if (sections.size() > 4) {
                try {
                    fiftyMovePlyCount = std::stoi(sections[4]);
                } catch (const std::invalid_argument& e) {
                    // Handle invalid number (e.g., empty or non-digit string)
                    fiftyMovePlyCount = 0;
                } catch (const std::out_of_range& e) {
                    // Handle number too large
                    fiftyMovePlyCount = 0;
                }
            }

            if (sections.size() > 5) {
                try {
                    moveCount = std::stoi(sections[5]);
                } catch (const std::invalid_argument& e) {
                    // Handle invalid number (e.g., empty or non-digit string)
                    moveCount = 0;
                } catch (const std::out_of_range& e) {
                    // Handle number too large
                    moveCount = 0;
                }
            }
        }
    };

    static PositionInfo positionFromFen(std::string fen) {
        PositionInfo loadedPositionInfo(fen);
        return loadedPositionInfo;
    }

    static std::string currentFen(const Board board, bool alwaysIncludeEnPassantSquare = true);

    /**
     * @brief checks if we can perform an enPassant capture in the current position of the board
     *
     * @param enPassantFileIndex
     * @param enPassantRankIndex
     * @param board
     * @return
     */
    static bool enPassantCanBeCaptured(int enPassantFileIndex, int enPassantRankIndex,
                                       const Board board);

    static std::string flipFen(std::string fen);

  private:
    Fen() = delete; // Static class, Prevents instantiation
    const static std::vector<std::string> split(const std::string& str, char delimiter = ' ') {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string item;

        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
        }
        return result;
    }
};
