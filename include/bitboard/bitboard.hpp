#pragma once

#include <cstdint>
#include <vector>

class BitBoard {

    static const uint64_t fileA = 0x101010101010101;
    static const auto notAFile = ~fileA;
    static const auto notHFile = ~(fileA << 7);

    static const uint64_t rank1 = 0b11111111;
    static const uint64_t rank2 = rank1 << 8;
    static const uint64_t rank3 = rank2 << 8;
    static const uint64_t rank4 = rank3 << 8;
    static const uint64_t rank5 = rank4 << 8;
    static const uint64_t rank6 = rank5 << 8;
    static const uint64_t rank7 = rank6 << 8;
    static const uint64_t rank8 = rank7 << 8;

    static std::vector<uint64_t> knightAttacks;
    static std::vector<uint64_t> kingMoves;
    static std::vector<uint64_t> whitePawnAttacks;
    static std::vector<uint64_t> blackPawnAttacks;
};
