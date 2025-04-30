
struct GameState {
  public:
    int capturedPieceType;
    int enPassantFile;
    int castlingRights;
    int fiftyMoveCounter;

    static const int ClearWhiteKingsideMask = 0b1110;
    static const int ClearWhiteQueensideMask = 0b1101;
    static const int ClearBlackKingsideMask = 0b1011;
    static const int ClearBlackQueensideMask = 0b0111;

    GameState(int _capturedPieceType, int _enPassantFile, int _castlingRights,
              int _fiftyMoveCounter) {
        capturedPieceType = _capturedPieceType;
        enPassantFile = _enPassantFile;
        castlingRights = _castlingRights;
        fiftyMoveCounter = _fiftyMoveCounter;
    }
    bool HasKingsideCastleRight(bool white) {
        int mask = white ? 1 : 4;
        return (castlingRights & mask) != 0;
    }

    bool HasQueensideCastleRight(bool white) {
        int mask = white ? 2 : 8;
        return (castlingRights & mask) != 0;
    }
};
