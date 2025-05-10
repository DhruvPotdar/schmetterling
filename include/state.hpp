#pragma once
/**
 * @file
 * @brief Contains struct for holding information about the GameState
 */

struct GameState {
  public:
    int capturedPieceClass;
    int enPassantFile;
    int castlingRights;
    int fiftyMoveCounter;

    static constexpr int clearWhiteKingsideMask = 0b1110;
    static constexpr int clearWhiteQueensideMask = 0b1101;
    static constexpr int clearBlackKingsideMask = 0b1011;
    static constexpr int clearBlackQueensideMask = 0b0111;

    GameState(int _capturedPieceType, int _enPassantFile, int _castlingRights,
              int _fiftyMoveCounter)
        : capturedPieceClass(_capturedPieceType), enPassantFile(_enPassantFile),
          castlingRights(_castlingRights), fiftyMoveCounter(_fiftyMoveCounter) {}

    bool HasKingsideCastleRight(bool white) {
        int mask = white ? 1 : 4;
        return (castlingRights & mask) != 0;
    }

    bool HasQueensideCastleRight(bool white) {
        int mask = white ? 2 : 8;
        return (castlingRights & mask) != 0;
    }

    GameState()
        : capturedPieceClass(0), enPassantFile(-1), // -1 often used to indicate no en passant
          castlingRights(0b1111),                   // assuming all rights available by default
          fiftyMoveCounter(0) {}
};
