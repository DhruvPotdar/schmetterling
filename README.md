# Schmetterling Chess Engine

**Schmetterling** is a chess engine under development, written in C++23. It aims to efficiently play chess using a bitboard-based approach. Currently, it includes board representation with bitboards, legal move generation, and a perft function for testing. Future updates will add position evaluation and search algorithms.

## Table of Contents
- [Features](#features)
- [Key Components](#key-components)
  - [Bitboards](#bitboards)
  - [Move Generation](#move-generation)
  - [Perft](#perft)
  - [Evaluation](#evaluation)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Using the Project with `just`](#using-the-project-with-just)
  - [Installing `just`](#installing-just)
  - [Available Tasks](#available-tasks)
  - [Examples](#examples)
- [Contributing](#contributing)
- [License](#license)

## Features
- Bitboard-based chess board representation.
- Legal move generation for all piece types.
- Perft function to test move generation correctness.
- Task automation using the `just` task runner.

## Key Components

### Bitboards
The chess board is represented using **bitboards**, which are 64-bit integers. Each bit corresponds to a square on the 8x8 chessboard (a1 = 0, h8 = 63). This allows for efficient operations like:
- Checking piece placement with bitwise AND.
- Generating moves for sliding pieces (rooks, bishops, queens) using shifts.

Bitboards are a cornerstone of modern chess engines due to their speed and compactness.

### Move Generation
The engine generates all **legal moves** from a given position, following chess rules. This includes:
- Standard moves for pawns, knights, bishops, rooks, queens, and kings.
- Special moves like castling, en passant, and pawn promotion.

Move generation is optimized with bitboards, using precomputed attack patterns where possible.

### Perft
**Perft** (Performance Test) calculates the total number of possible move sequences up to a specified depth. It’s a debugging tool to ensure move generation is correct. For example, in the starting position:
- Depth 1: 20 moves.
- Depth 5: 4,865,609 nodes.

This matches results from established engines, verifying accuracy.

### Evaluation
**Evaluation** is not yet implemented but is planned for future releases. It will assign a numerical score to a position, reflecting its strength for the player to move. Factors will include:
- Piece values (e.g., pawn = 1, queen = 9).
- Pawn structure, king safety, and mobility.

Stay tuned for updates!

## Getting Started

### Prerequisites
- **CMake**: Version 3.10 or higher.
- **C++23 Compiler**: GCC 12+, Clang 15+, or equivalent.
- **just**: Task runner (see installation below).

### Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/schmetterling.git
   cd schmetterling
   ```
2. Build the project using `just` (see below).

## Using the Project with `just`

### Installing `just`
`just` is a task runner that simplifies building, testing, and running the project. Install it with:
- **Cargo** (Rust package manager):
  ```bash
  cargo install just
  ```
- **Package Manager**:
  - macOS: `brew install just`
  - Linux: Check your distro’s package manager or build from source.

Verify installation:
```bash
just --version
```

### Available Tasks
The `justfile` defines these tasks:
- **`just build`**: Compiles the project in Release mode.
- **`just run`**: Builds and runs the main executable.
- **`just perft [depth]`**: Runs perft with a specified depth (default: 5).
- **`just clean`**: Removes build artifacts.

### Examples
- Build the project:
  ```bash
  just build
  ```
- Run the engine:
  ```bash
  just run
  ```
- Test move generation with perft at depth 4:
  ```bash
  just perft 4
  ```
- Clean the build directory:
  ```bash
  just clean
  ```

Run `just --list` to see all tasks.

## Contributing
Contributions are welcome! Fork the repo, create a branch, and submit a pull request with your changes.

## License
MIT License. See [LICENSE](LICENSE) for details.
