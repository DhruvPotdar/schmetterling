#include "board/bitboard.hpp"
#include <iostream>

void BoardState::setSquare(piece p, uint32_t index) {
  boardMap.at(p) |= 1 << index;
}

void BoardState::capture(uint32_t index) { boardMap.at(p) &= !(1 << index); }
square getSquare(uint32_t index) { return index; }

int main() {
  BoardState b;
  auto p = P;
  auto index = 1;
  std::cout << b.getPiece(p) << std::endl;
  b.setSquare(p, index);

  std::cout << b.getPiece(p);
}
