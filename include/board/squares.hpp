#pragma once

#include <array>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>

/**
 * @struct Offset
 * Represents a displacement in file (x) and rank (y) directions on the chessboard.
 */
struct Offset {
    int file; ///< File displacement (0 = no change, positive = right, negative = left)
    int rank; ///< Rank displacement (0 = no change, positive = up, negative = down)

    constexpr Offset(int fileOffset = 0, int rankOffset = 0) : file(fileOffset), rank(rankOffset) {}
};

/**
 * @class Square
 * Represents a square on the chess board using a 0..63 index or None (-1).
 * Supports conversion to/from algebraic notation and a variety of utility methods.
 */
class Square {
  private:
    int index_; // -1 for None, 0-63 for A1-H8

  public:
    static const Square None;

    // Constructors
    constexpr Square() : index_(-1) {}

    constexpr Square(const int index) {
        if (index < -1 || index > 63) {
            throw std::out_of_range("Square index must be between -1 and 63");
        }
        index_ = index;
    }

    constexpr Square(const int file, const int rank) {
        if (file < 0 || file > 7 || rank < 0 || rank > 7) {
            throw std::out_of_range("File and rank must be between 0 and 7");
        }
        index_ = file + rank * 8;
    }

    constexpr Square(const char fileChar, const int rank) {
        if (fileChar < 'A' || fileChar > 'H' || rank < 1 || rank > 8) {
            throw std::out_of_range("Invalid file or rank");
        }
        index_ = (fileChar - 'A') + (rank - 1) * 8;
    }

    constexpr Square(const std::string& algebraic) {
        auto algebraicNotation = algebraic;
        algebraicNotation[0] = std::toupper(algebraicNotation[0]);

        if (algebraicNotation.size() != 2 || algebraicNotation[0] < 'A' ||
            algebraicNotation[0] > 'H' || algebraicNotation[1] < '1' ||
            algebraicNotation[1] > '8') {
            throw std::invalid_argument("Invalid algebraic notation");
        }

        const auto file = algebraicNotation[0] - 'A';
        const auto rank = algebraicNotation[1] - '1';

        index_ = file + rank * 8;
    }

    // Index getter
    inline constexpr int getIndex() const { return index_; }

    // File/rank getters
    inline constexpr uint8_t getFile() const {
        assert(*this != Square::None && "Cannot get file of None");
        return index_ % 8;
    }

    inline constexpr int getRankIndex() const {
        assert(*this != Square::None && "Cannot get rank of None");
        return index_ / 8;
    }

    inline constexpr char getFileChar() const { return 'A' + getFile(); }
    inline constexpr int getRank() const { return getRankIndex() + 1; }

    // Algebraic
    std::string getAlgebraic() const {
        static const std::array<std::string, 64> cache = []() {
            std::array<std::string, 64> tmp;
            for (int i = 0; i < 64; ++i) {
                char f = 'A' + (i % 8);
                char r = '1' + (i / 8);
                tmp[i] = std::string{f} + r;
            }
            return tmp;
        }();
        assert(*this != Square::None && "Cannot get algebraic of None");
        return cache[index_];
    }

    // Color check
    inline constexpr bool isLightSquare() const {
        assert(*this != Square::None && "Cannot check color of None");
        return ((getFile() + getRankIndex()) % 2) != 0;
    }

    inline constexpr bool isValid() const { return index_ >= 0 && index_ < 64; }

    // Offset operations
    inline constexpr Square offset(const Offset& d) const {
        assert(*this != Square::None && "Cannot offset from None");
        const auto fx = getFile() + d.file;
        const auto ry = getRankIndex() + d.rank;
        if (fx < 0 || fx > 7 || ry < 0 || ry > 7) {
            throw std::out_of_range("Offset leads outside the board");
        }
        return Square{fx, ry};
    }

    inline constexpr Square tryOffset(const Offset& d) const {
        if (*this == Square::None) return Square::None;
        const auto fx = getFile() + d.file;
        const auto ry = getRankIndex() + d.rank;
        if (fx < 0 || fx > 7 || ry < 0 || ry > 7) return Square::None;
        return Square{fx, ry};
    }

    // Arithmetic with Offset
    inline constexpr Square operator+(const Offset& d) const { return offset(d); }
    inline constexpr Square operator-(const Offset& d) const {
        return offset(Offset{-d.file, -d.rank});
    }
    inline constexpr Square operator*(int m) const {
        return offset(Offset{getFile() * (m - 1), getRankIndex() * (m - 1)});
    }

    // Distance (Manhattan)
    inline constexpr uint8_t distanceTo(const Square& o) const {
        if (*this == Square::None || o == Square::None) {
            return std::numeric_limits<uint8_t>::max();
        }
        const auto dx = std::abs(getFile() - o.getFile());
        const auto dy = std::abs(getRankIndex() - o.getRankIndex());
        return static_cast<uint8_t>(dx + dy);
    }

    // Chess relations
    inline constexpr bool sameFile(const Square& o) const {
        return isValid() && o.isValid() && getFile() == o.getFile();
    }
    inline constexpr bool sameRank(const Square& o) const {
        return isValid() && o.isValid() && getRank() == o.getRank();
    }
    inline constexpr bool sameDiagonal(const Square& o) const {
        return isValid() && o.isValid() &&
               std::abs(getFile() - o.getFile()) == std::abs(getRankIndex() - o.getRankIndex());
    }

    // Conversion and comparison
    inline constexpr explicit operator int() const { return index_; }
    inline constexpr bool operator==(const Square& o) const { return index_ == o.index_; }
    inline constexpr bool operator!=(const Square& o) const { return !(*this == o); }

    // Increment/decrement (index-based)
    inline constexpr Square& operator++() {
        if (index_ < 63) ++index_;
        return *this;
    }

    inline constexpr Square operator++(int) {
        Square tmp = *this;
        ++(*this);
        return tmp;
    }

    inline constexpr Square& operator--() {
        if (index_ > -1) --index_;
        return *this;
    }

    inline constexpr Square operator--(int) {
        Square tmp = *this;
        --(*this);
        return tmp;
    }
};

inline const Square Square::None(-1);
