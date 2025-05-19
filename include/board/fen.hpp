#pragma once

#include "squares.hpp"
#include "types.hpp"
#include <optional>
#include <string>

class Board; // Forward declaration

class FEN {
  public:
    static void parse(const std::string& fen, Board& board);
    static std::string generate(const Board& board);

  private:
    static void parsePiecePlacement(const std::string& placement, Board& board);
    static Side parseActiveColor(const std::string& color);
    static uint8_t parseCastlingRights(const std::string& castling);
    static std::optional<Square> parseEnPassant(const std::string& enPassant);
    static int parseHalfMoveClock(const std::string& halfMove);
    static int parseFullMoveNumber(const std::string& fullMove);
    static std::string generatePiecePlacement(const Board& board);
    static std::string generateActiveColor(const Board& board);
    static std::string generateCastlingRights(const Board& board);
    static std::string generateEnPassant(const Board& board);
    static std::string generateHalfMoveClock(const Board& board);
    static std::string generateFullMoveNumber(const Board& board);
};
