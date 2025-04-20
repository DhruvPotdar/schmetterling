//   // Piece bitboards
//   /**********************************\
//    ==================================
//
//               Chess board
//
//    ==================================
//   \**********************************/
//
//   /*
//                               WHITE PIECES
//
//
//           Pawns                  Knights              Bishops
//
//     8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0
//     7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
//     6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
//     5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
//     4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
//     3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
//     2  1 1 1 1 1 1 1 1    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
//     1  0 0 0 0 0 0 0 0    1  0 1 0 0 0 0 1 0    1  0 0 1 0 0 1 0 0
//
//        a b c d e f g h       a b c d e f g h       a b c d e f g h
//
//
//            Rooks                 Queens                 King
//
//     8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0
//     7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
//     6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
//     5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
//     4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
//     3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
//     2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
//     1  1 0 0 0 0 0 0 1    1  0 0 0 1 0 0 0 0    1  0 0 0 0 1 0 0 0
//
//        a b c d e f g h       a b c d e f g h       a b c d e f g h
//
//
//                               BLACK PIECES
//
//
//           Pawns                  Knights              Bishops
//
//     8  0 0 0 0 0 0 0 0    8  0 1 0 0 0 0 1 0    8  0 0 1 0 0 1 0 0
//     7  1 1 1 1 1 1 1 1    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
//     6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
//     5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
//     4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
//     3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
//     2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
//     1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0
//
//        a b c d e f g h       a b c d e f g h       a b c d e f g h
//
//
//            Rooks                 Queens                 King
//
//     8  1 0 0 0 0 0 0 1    8  0 0 0 1 0 0 0 0    8  0 0 0 0 1 0 0 0
//     7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
//     6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
//     5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
//     4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
//     3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
//     2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
//     1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0
//
//        a b c d e f g h       a b c d e f g h       a b c d e f g h
//
//
//
//                                OCCUPANCIES
//
//
//        White occupancy       Black occupancy       All occupancies
//
//     8  0 0 0 0 0 0 0 0    8  1 1 1 1 1 1 1 1    8  1 1 1 1 1 1 1 1
//     7  0 0 0 0 0 0 0 0    7  1 1 1 1 1 1 1 1    7  1 1 1 1 1 1 1 1
//     6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
//     5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
//     4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
//     3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
//     2  1 1 1 1 1 1 1 1    2  0 0 0 0 0 0 0 0    2  1 1 1 1 1 1 1 1
//     1  1 1 1 1 1 1 1 1    1  0 0 0 0 0 0 0 0    1  1 1 1 1 1 1 1 1
//
//
//
//                               ALL TOGETHER
//
//                           8  ♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜
//                           7  ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎
//                           6  . . . . . . . .
//                           5  . . . . . . . .
//                           4  . . . . . . . .
//                           3  . . . . . . . .
//                           2  ♙ ♙ ♙ ♙ ♙ ♙ ♙ ♙
//                           1  ♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖
//
//                              a b c d e f g h
//
//   */
#ifndef BITBOARD_HPP
#define BITBOARD_HPP
#include <cstdint>
#include <map>
#include <string>
#include <sys/types.h>
typedef uint64_t board;
typedef bool square;

// Piece Names
enum piece {
  P,
  N,
  B,
  R,
  Q,
  K,
  p,
  n,
  b,
  r,
  q,
  k,
};
// TODO: Add to the boardMap for easy usage
static std::string unicode_pieces[12] = {"",  "",  "",  "",
                                         "",  "",  "",  "󰡘",
                                         "󰡜", "󰡛", "󰡚", "󰡗"};

// Pieces
// Containts the Current positoin of all the pieces on the board
class BoardState {
  // Start with default positions of chess
private:
  board white_pawns = 0x000000000000FF00ULL;
  board white_knights = 0x0000000000000042ULL;
  board white_bishops = 0x0000000000000024ULL;
  board white_rooks = 0x0000000000000081ULL;
  board white_queens = 0x0000000000000008ULL;
  board white_king = 0x0000000000000010ULL;
  board white_pieces = white_pawns | white_knights | white_bishops |
                       white_rooks | white_queens | white_king;

  board black_pawns = 0x00FF000000000000ULL;
  board black_knights = 0x4200000000000000ULL;
  board black_bishops = 0x2400000000000000ULL;
  board black_rooks = 0x8100000000000000ULL;
  board black_queens = 0x0800000000000000ULL;
  board black_king = 0x1000000000000000ULL;
  board black_pieces = black_pawns | black_knights | black_bishops |
                       black_rooks | black_queens | black_king;

  // Unified Board map for all mappings from piece to any other type(icons,
  // symbols, strings etc)
  std::map<piece, board> boardMap = {
      {P, white_pawns},
      {N, white_knights},
      {B, white_bishops},
      {R, white_rooks},
      {Q, white_queens},
      {K, white_king},
      // {WHITE_PIECES, white_pieces},
      {p, black_pawns},
      {n, black_knights},
      {b, black_bishops},
      {r, black_rooks},
      {q, black_queens},
      {k, black_king},
      // {BLACK_PIECES, black_pieces}
  };

public:
  BoardState() {};
  void setSquare(piece p, uint32_t index);
  square getSquare(uint32_t index);
  void capture(uint32_t index);
  size_t getPieceCount();
  board getPiece(piece p) { return boardMap.at(p); }

  BoardState operator~() const;
  BoardState operator&(const board &other) const;
  BoardState operator|(const board &other) const;
  BoardState operator^(const board &other) const;
  BoardState operator&=(const board &other) const;
  BoardState operator|=(const board &other) const;
  BoardState operator^=(const board &other) const;
};

//
//
//
//
//
//
//
//
//
#endif // BITBOARD_HPP
