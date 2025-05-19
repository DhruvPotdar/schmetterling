#pragma once

#include "squares.hpp"
#include <array>
#include <cstdint>
#include <stdexcept>

class BitBoard {
  private:
    uint64_t _bits;

  public:
    // Default & explicit constructor
    constexpr BitBoard(uint64_t bits = 0ULL) : _bits(bits) {}

    // Conversion to raw bitboard
    constexpr operator uint64_t() const { return _bits; }

    // Bitwise operators
    constexpr BitBoard operator&(BitBoard other) const { return BitBoard{_bits & other._bits}; }
    constexpr BitBoard operator|(BitBoard other) const { return BitBoard{_bits | other._bits}; }
    constexpr BitBoard operator^(BitBoard other) const { return BitBoard{_bits ^ other._bits}; }

    BitBoard& operator&=(BitBoard other) {
        _bits &= other._bits;
        return *this;
    }
    BitBoard& operator|=(BitBoard other) {
        _bits |= other._bits;
        return *this;
    }
    BitBoard& operator^=(BitBoard other) {
        _bits ^= other._bits;
        return *this;
    }

    // Set, clear, toggle a single square (if valid)
    void set(Square square) {
        if (!square.isValid()) throw std::out_of_range("Cannot set BitBoard: invalid square");
        _bits |= (1ULL << square.getIndex());
    }

    void clear(Square square) {
        if (!square.isValid()) throw std::out_of_range("Cannot clear BitBoard: invalid square");
        _bits &= ~(1ULL << square.getIndex());
    }

    void toggle(Square square) {
        if (!square.isValid()) throw std::out_of_range("Cannot toggle BitBoard: invalid square");
        _bits ^= (1ULL << square.getIndex());
    }

    bool contains(Square square) const {
        return square.isValid() && ((_bits >> square.getIndex()) & 1ULL);
    }

    // Shift bitboard by raw offsets (positive = left shift; negative = right shift)
    constexpr BitBoard shift(int offset) const {
        if (offset > 0) {
            return BitBoard{_bits << offset};
        } else {
            return BitBoard{_bits >> -offset};
        }
    }

    // Convenience: single-square bitboard
    static constexpr BitBoard of(Square square) {
        return square.isValid() ? BitBoard{1ULL << square.getIndex()} : BitBoard{0ULL};
    }
};

// Precomputed bitboards for each square A1..H8
static constexpr auto squareBB = []() {
    std::array<BitBoard, 64> arr{};
    for (int i = 0; i < 64; ++i) {
        arr[i] = BitBoard{1ULL << i};
    }
    return arr;
}();

// Precomputed ranks (1–8)
static constexpr auto rankBB = []() {
    std::array<BitBoard, 8> arr{};
    for (int r = 0; r < 8; ++r) {
        arr[r] = BitBoard{0xFFULL << (r * 8)};
    }
    return arr;
}();

// Precomputed files (A–H)
static constexpr auto fileBB = []() {
    std::array<BitBoard, 8> arr{};
    for (int f = 0; f < 8; ++f) {
        arr[f] = BitBoard{0x0101010101010101ULL << f};
    }
    return arr;
}();
