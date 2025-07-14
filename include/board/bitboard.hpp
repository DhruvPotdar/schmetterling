#pragma once

#include "squares.hpp"
#include <array>
#include <cstdint>
#include <sstream>
#include <stdexcept>
class Board;

class BitBoard {
  private:
    uint64_t _bits;

  public:
    // Default & explicit constructor
    inline constexpr BitBoard(uint64_t bits = 0ULL) : _bits(bits) {}

    // Conversion to raw bitboard
    inline constexpr operator uint64_t() const { return _bits; }
    inline constexpr operator std::string() const { return this->createDiagram(); }

    // Bitwise operators
    inline constexpr BitBoard operator&(BitBoard other) const {
        return BitBoard{_bits & other._bits};
    }
    inline constexpr BitBoard operator|(BitBoard other) const {
        return BitBoard{_bits | other._bits};
    }
    inline constexpr BitBoard operator^(BitBoard other) const {
        return BitBoard{_bits ^ other._bits};
    }
    inline constexpr BitBoard operator~() const { return BitBoard{~_bits}; }

    inline constexpr BitBoard& operator&=(BitBoard other) {
        _bits &= other._bits;
        return *this;
    }

    inline constexpr BitBoard& operator|=(BitBoard other) {
        _bits |= other._bits;
        return *this;
    }

    inline constexpr BitBoard& operator^=(BitBoard other) {
        _bits ^= other._bits;
        return *this;
    }

    inline constexpr int LSBIndex() const { return __builtin_ctzll(_bits); }

    inline constexpr bool isEmpty() const { return _bits == 0; }

    inline constexpr Square popMSB() {
        if (_bits == 0) return Square::None; // No bits set
        const auto index = 63 - __builtin_clzll(_bits);
        _bits &= ~(1ULL << index); // Clear the MSB
        return Square(index);
    }
    inline constexpr Square popLSB() {
        if (_bits == 0) return Square::None; // No bits set
        const auto index = __builtin_ctzll(_bits);
        _bits &= _bits - 1;
        return Square(index);
    }
    inline constexpr int popCount() const { return __builtin_popcountll(_bits); }

    // Set, clear, toggle a single square (if valid)
    inline constexpr void set(Square square) {
        if (!square.isValid()) throw std::out_of_range("Cannot set BitBoard: invalid square");
        _bits |= (1ULL << square.getIndex());
    }

    inline constexpr void clear(Square square) {
        if (!square.isValid()) throw std::out_of_range("Cannot clear BitBoard: invalid square");
        _bits &= ~(1ULL << square.getIndex());
    }

    inline constexpr void toggle(Square square) {
        if (!square.isValid()) throw std::out_of_range("Cannot toggle BitBoard: invalid square");
        _bits ^= (1ULL << square.getIndex());
    }

    inline constexpr bool contains(Square square) const {
        return square.isValid() && ((_bits >> square.getIndex()) & 1ULL);
    }

    // Shift bitboard by raw offsets (positive = left shift; negative = right shift)
    inline constexpr BitBoard shift(int offset) const {
        if (offset > 0) {
            return BitBoard{_bits << offset};
        } else {
            return BitBoard{_bits >> -offset};
        }
    }

    // Convenience: single-square bitboard
    inline static constexpr BitBoard of(Square square) {
        return square.isValid() ? BitBoard{1ULL << square.getIndex()} : BitBoard{0ULL};
    }

    static constexpr std::string BLACK_BG = "\033[1;48;5;66m";  // Dark teal square (#5f8787)
    static constexpr std::string WHITE_BG = "\033[1;48;5;151m"; // Light green square (#afffaf)
    static constexpr std::string BLACK_FG = "\033[1;38;5;232m"; // Very dark gray (#080808)
    static constexpr std::string WHITE_FG = "\033[1;38;5;231m"; // Bright white
    static constexpr std::string RESET = "\033[0m";

    std::string createDiagram() const {
        std::ostringstream ss;

        // Board frame top
        ss << "  ┌────────────────────────┐\n";

        // Print 8x8 grid (top-down, like Board::createDiagram with blackAtTop=true)
        for (int rank = 7; rank >= 0; --rank) {
            ss << (rank + 1) << " │";
            for (int file = 0; file < 8; ++file) {
                Square sq(file, rank);
                bool isLightSquare = (file + rank) % 2 != 0;
                bool isSet = contains(sq);

                // Set background color
                ss << (isLightSquare ? WHITE_BG : BLACK_BG);

                // Set foreground color and symbol
                if (isSet) {
                    ss << BLACK_FG << " ● ";
                } else {
                    ss << WHITE_FG << " ● ";
                }

                ss << RESET;
            }
            ss << "│\n";
        }

        // Board frame bottom
        ss << "  └────────────────────────┘\n";

        // File labels
        ss << "    a  b  c  d  e  f  g  h\n";

        // Optional: Add raw bit value for debugging
        ss << "Bits: 0x" << std::hex << _bits << std::dec << "\n";

        return ss.str();
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
