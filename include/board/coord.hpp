#pragma once
/**
 * @class Coord
 * @brief Class for representing squares on the chess board as file/rank integer pairs. (0, 0) = a1,
 * (7, 7) = h8. Coords can also be used as offsets. For example, while a Coord of (-1, 0) is not a
 * valid square, it can be used to represent the concept of moving 1 square left.
 */
/**
 * @class Coord
 * @brief define chessboard coordinates
 * x: file index
 * y: rank index
 *
 */
class Coord {
  public:
    const int x = 0; // file index
    const int y = 0; // rank index

    Coord(int x, int y) : x(x), y(y) {}

    // Converts the square index to board coords
    Coord(int squareIndex) : x(squareIndex & 0b000111), y(squareIndex >> 3) {}

    bool isLightSquare() const { return (x + y) % 2 != 0; }

    int compareTo(const Coord& other) const { return (x == other.x && y == other.y) ? 0 : 1; }

    bool isValidSquare() const { return x >= 0 && x < 8 && y >= 0 && y < 8; }

    int squareIndex() const { return this->y * 8 + this->x; }

    Coord operator+(const Coord& other) const { return Coord(x + other.x, y + other.y); }

    Coord operator-(const Coord& other) const { return Coord(x - other.x, y - other.y); }

    Coord operator*(int m) const { return Coord(x * m, y * m); }

    friend Coord operator*(int m, const Coord& a) { return a * m; }
};
