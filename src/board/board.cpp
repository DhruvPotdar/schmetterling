#include "board/board.hpp"

#include "bitboard/bitBoardUtil.hpp"
#include "board/piece.hpp"
#include "helpers/fen.hpp"
#include "state.hpp"

#include <bitset>
#include <iostream>
#include <memory>

std::array<board, 64> BitBoardUtil::knightAttacks;
std::array<board, 64> BitBoardUtil::whitePawnAttacks;
std::array<board, 64> BitBoardUtil::blackPawnAttacks;

std::string Board::getCurrentFen() { return Fen::currentFen(*this, true); }

void Board::makeMove(Move move, bool inSearch = false) {

    // Get info about the move
    const auto startSquare = move.StartSquare();
    const auto targetSquare = move.targetSquare();
    const auto moveFlag = move.GetMoveFlag();
    const auto isPromotion = move.IsPromotion();
    const bool isEnPassant = moveFlag == MoveFlag::EnPassantCaptureFlag;

    const auto movedPiece = square[startSquare];
    const auto movedPieceClass = Piece::pieceClass(movedPiece);
    const auto capturedPiece =
        isEnPassant ? Piece::makePiece(Piece::Pawn, getOpponentColor()) : square[targetSquare];
    const auto capturedPieceClass = Piece::pieceClass(capturedPiece);

    // Castling and EnPassant
    const auto prevCastleState = currentGameState.castlingRights;
    const auto prevEnPassantFile = currentGameState.enPassantFile;
    auto newCastlingRights = currentGameState.castlingRights;
    auto newEnPassantFile = 0;

    Board::movePiece(movedPiece, startSquare, targetSquare);

    // Captures
    if (capturedPieceClass != Piece::None) {
        auto captureSquare = targetSquare;
        if (isEnPassant) {
            captureSquare = targetSquare + (isWhiteToMove ? -8 : 8);
            square[captureSquare] = Piece::None;
        }
        if (capturedPieceClass != Piece::Pawn) {
            totalPieceCountWithoutPawnsAndKings--;
        }

        // Remove Captured piece from bitboard and piece piecelist
        allPieceLists[capturedPiece].removePieceAtSquare(captureSquare);
        BitBoardUtil::toggleSquare(piecesBitBoards[capturedPiece], captureSquare);
        BitBoardUtil::toggleSquare(colorBitboards[capturedPiece], captureSquare);
    }

    // Handle Kings
    if (movedPieceClass == Piece::King) {
        kingSquare[getMoveColorIndex()] = targetSquare;
        newCastlingRights &= (isWhiteToMove) ? 0b1100 : 0b0011;

        // Handle castling
        if (moveFlag == MoveFlag::CastleFlag) {
            const auto rook = Piece::makePiece(Piece::Rook, getMoveColor());
            const auto kingSide = targetSquare == Board::g1 || targetSquare == Board::g8;

            const auto castlingRookFromIndex = (kingSide) ? targetSquare + 1 : targetSquare - 2;
            const auto castlingRookToIndex = (kingSide) ? targetSquare - 1 : targetSquare + 1;

            BitBoardUtil::toggleSquares(piecesBitBoards[rook], castlingRookFromIndex,
                                        castlingRookToIndex);
            BitBoardUtil::toggleSquares(colorBitboards[getMoveColorIndex()], castlingRookFromIndex,
                                        castlingRookToIndex);
            allPieceLists[rook].movePiece(castlingRookFromIndex, castlingRookToIndex);
            square[castlingRookFromIndex] = Piece::None;
            square[castlingRookToIndex] = Piece::Rook | getMoveColor();
        }
    }

    // Handle Promotion
    if (isPromotion) {
        totalPieceCountWithoutPawnsAndKings++;

        const int promotionPieceClass = [promotionPieceClass, &moveFlag] {
            switch (moveFlag) {
            case MoveFlag::PromoteToQueenFlag:
                return Piece::Queen;
            case MoveFlag::PromoteToRookFlag:
                return Piece::Rook;
            case MoveFlag::PromoteToKnightFlag:
                return Piece::Knight;
            case MoveFlag::PromoteToBishopFlag:
                return Piece::Bishop;
            default:
                return Piece::None;
            }
        }();
        piece_t promotionPiece = Piece::makePiece(promotionPieceClass, getMoveColor());

        // Remove pawn from promotion square and add promoted piece instead
        BitBoardUtil::toggleSquare(piecesBitBoards[movedPiece], targetSquare);
        BitBoardUtil::toggleSquare(piecesBitBoards[promotionPiece], targetSquare);

        allPieceLists[movedPiece].removePieceAtSquare(targetSquare);
        allPieceLists[promotionPiece].addPieceAtSquare(targetSquare);
        square[targetSquare] = promotionPiece;
    }
    // Pawn has moved two forwards, mark file with en-passant flag
    if (moveFlag == MoveFlag::PawnTwoUpFlag) {
        const auto file = Board::fileIndex(startSquare) + 1;
        newEnPassantFile = file;
    }

    // Update Castling rights
    if (prevCastleState != 0) {
        if (targetSquare == Board::h1 || startSquare == Board::h1) {
            newCastlingRights &= GameState::clearWhiteKingsideMask;
        } else if (targetSquare == Board::a1 || startSquare == Board::a1) {
            newCastlingRights &= GameState::clearWhiteQueensideMask;
        }
        if (targetSquare == Board::h8 || startSquare == Board::h8) {
            newCastlingRights &= GameState::clearBlackKingsideMask;
        } else if (targetSquare == Board::a8 || startSquare == Board::a8) {
            newCastlingRights &= GameState::clearBlackQueensideMask;
        }
    }

    // Change Side to move
    isWhiteToMove = !isWhiteToMove;

    plyCount++;
    int newFiftyMoveCounter = currentGameState.fiftyMoveCounter + 1;

    // Update extra bitboards
    allPiecesBitboard = colorBitboards[whiteIndex] | colorBitboards[blackIndex];
    updateSliderBitboards();

    if (movedPiece == Piece::Pawn || capturedPieceClass != Piece::None) {
        if (!inSearch) {
            // Clear the stack by popping all elements
            // while (!repititionPositionHistory.empty()) {
            //     repititionPositionHistory.pop();
            // }
        }
        newFiftyMoveCounter = 0;
    }
    GameState newState(capturedPieceClass, newEnPassantFile, newCastlingRights, newFiftyMoveCounter);
    gameStateHistory.push_back(newState);

    currentGameState = newState;
    hasCachedInCheckValue = false;

    if (!inSearch) {
        allGameMoves.push_back(move);
    }
}

void Board::unmakeNullMove() {
    isWhiteToMove = !isWhiteToMove;
    plyCount--;
    gameStateHistory.pop_back();
    currentGameState = gameStateHistory.back();
    updateSliderBitboards();
    hasCachedInCheckValue = true;
    cachedInCheckValue = false;
}
void Board::unmakeMove(Move move, bool inSearch = false) {
    // Swap color to move
    isWhiteToMove = !isWhiteToMove;

    const bool undoingWhiteMove = isWhiteToMove;

    // Get move info
    //
    const auto movedFrom = move.StartSquare();
    const auto movedTo = move.targetSquare();
    const auto movedFlag = move.GetMoveFlag();

    const auto undoingEnPassant = movedFlag == MoveFlag::EnPassantCaptureFlag;
    const auto undoingPromotion = move.IsPromotion();
    const auto undoingCapture = currentGameState.capturedPieceClass != Piece::None;

    const auto movedPiece =
        undoingPromotion ? Piece::makePiece(Piece::Pawn, getMoveColor()) : square[movedTo];
    const auto movedPieceClass = Piece::pieceClass(movedPiece);
    const auto capturedPieceClass = currentGameState.capturedPieceClass;

    // remove piece from promotion square and replace the square with a pawn
    if (undoingPromotion) {

        const auto promotedPiece = square[movedTo];
        const auto pawnPiece = Piece::makePiece(Piece::Pawn, getMoveColor());
        totalPieceCountWithoutPawnsAndKings--;

        allPieceLists[promotedPiece].removePieceAtSquare(movedTo);
        allPieceLists[movedPiece].addPieceAtSquare(movedTo);
        BitBoardUtil::toggleSquare(piecesBitBoards[promotedPiece], movedTo);
        BitBoardUtil::toggleSquare(piecesBitBoards[pawnPiece], movedTo);
    }

    movePiece(movedPiece, movedTo, movedFrom);

    // Undo Capture
    if (undoingCapture) {
        auto captureSquare = movedTo;
        const auto capturedPiece = Piece::makePiece(capturedPieceClass, getOpponentColor());
        if (undoingEnPassant) {
            captureSquare = movedTo + ((undoingWhiteMove) ? -8 : 8);
        }
        if (capturedPieceClass != Piece::Pawn) {
            totalPieceCountWithoutPawnsAndKings++;
        }

        // Add back captured piece
        BitBoardUtil::toggleSquare(piecesBitBoards[capturedPiece], captureSquare);
        BitBoardUtil::toggleSquare(colorBitboards[getOpponentMoveColorIndex()], captureSquare);
        allPieceLists[capturedPiece].addPieceAtSquare(captureSquare);
        square[captureSquare] = capturedPiece;
    }

    if (movedPieceClass == Piece::King) {

        kingSquare[getMoveColorIndex()] = movedFrom;

        // undo castling
        if (movedFlag == MoveFlag::CastleFlag) {

            const auto rookPiece = Piece::makePiece(Piece::Rook, getMoveColor());
            const auto kingSide = movedTo == Board::g1 || movedTo == Board::g8;

            const auto rookSquareBeforeCastling = kingSide ? movedTo + 1 : movedTo - 2;
            const auto rookSquareAfterCastling = kingSide ? movedTo - 1 : movedTo + 1;

            // Undo castling by returing rook to orignal square
            BitBoardUtil::toggleSquares(piecesBitBoards[rookPiece], rookSquareAfterCastling,
                                        rookSquareBeforeCastling);
            BitBoardUtil::toggleSquares(colorBitboards[getMoveColorIndex()], rookSquareAfterCastling,
                                        rookSquareBeforeCastling);

            square[rookSquareAfterCastling] = Piece::None;
            square[rookSquareBeforeCastling] = rookPiece;

            allPieceLists[rookPiece].movePiece(rookSquareAfterCastling, rookSquareBeforeCastling);
        }
    }

    allPiecesBitboard = colorBitboards[whiteIndex] | colorBitboards[blackIndex];
    updateSliderBitboards();

    // if (!inSearch && repititionPositionHistory.empty()) {
    //     repititionPositionHistory.pop();
    // }
    if (!inSearch) {
        allGameMoves.pop_back();
    }
    if (!gameStateHistory.empty()) {

        gameStateHistory.pop_back();
        currentGameState = gameStateHistory.back();
    } else {
        currentGameState = GameState();
    }
    currentGameState = gameStateHistory.back();
    plyCount--;
    cachedInCheckValue = false;
}

void Board::makeNullMove() {
    isWhiteToMove = !isWhiteToMove;
    plyCount--;
    gameStateHistory.pop_back();
    currentGameState = gameStateHistory.back();
    updateSliderBitboards();
    hasCachedInCheckValue = true;
    cachedInCheckValue = false;
}

/**
 * @brief Is current player in check?
 * Note: caches check value so calling multiple times does not require recalculating
 * @return
 **/

bool Board::isInCheck() {
    if (hasCachedInCheckValue) {
        return cachedInCheckValue;
    }
    cachedInCheckValue = calculateInCheckState();
    hasCachedInCheckValue = true;

    return cachedInCheckValue;
}

bool Board::calculateInCheckState() {
    const auto _kingSquare = kingSquare[getMoveColorIndex()];
    const auto blockers = allPiecesBitboard;

    if (enemyOrthogonalSliders != 0) {
        // const auto rookAttacks = GetRookAttacks();
        if (
            // (   rookAttacks *
            enemyOrthogonalSliders
            // )
            != 0)
            return true;
    }

    if (enemyDiagonalSliders != 0) {
        // const auto bishopAttacks = GetBishopAttacks();
        if (
            // (   bishopAttacks *
            enemyDiagonalSliders
            // )
            != 0)
            return true;
    }
    const auto enemyKnights = piecesBitBoards[Piece::makePiece(Piece::Knight, getOpponentColor())];
    if ((BitBoardUtil::knightAttacks[_kingSquare] & enemyKnights) != 0) return true;
    const auto enemyPawns = piecesBitBoards[Piece::makePiece(Piece::Pawn, getOpponentColor())];
    const auto pawnAttackMask = isWhiteToMove ? BitBoardUtil::whitePawnAttacks[_kingSquare]
                                              : BitBoardUtil::blackPawnAttacks[_kingSquare];
    if ((pawnAttackMask & enemyPawns) != 0) {
        return true;
    }

    return false;
}

void Board::updateSliderBitboards() {
    const auto friendlyRook = Piece::makePiece(Piece::Rook, getMoveColor());
    const auto friendlyQueen = Piece::makePiece(Piece::Queen, getMoveColor());
    const auto friendlyBishop = Piece::makePiece(Piece::Bishop, getMoveColor());
    friendlyOrthogonalSliders = piecesBitBoards[friendlyRook] || piecesBitBoards[friendlyQueen];
    friendlyDiagonalSliders = piecesBitBoards[friendlyBishop] || piecesBitBoards[friendlyQueen];

    const auto enemyRook = Piece::makePiece(Piece::Rook, getMoveColor());
    const auto enemyQueen = Piece::makePiece(Piece::Queen, getMoveColor());
    const auto enemyBishop = Piece::makePiece(Piece::Bishop, getMoveColor());
    enemyOrthogonalSliders = piecesBitBoards[enemyRook] || piecesBitBoards[enemyQueen];
    enemyDiagonalSliders = piecesBitBoards[enemyBishop] || piecesBitBoards[enemyQueen];
}

/**
 * @brief Update piece lists / bitboards based on given move info. Note that this does not
account for the following things, which must be handled separately:
1. Removal of a captured piece
2. Movement of rook when castling
3. Removal of pawn from 1st/8th rank during pawn promotion
4. Addition of promoted piece during pawn promotion
 *
 * @param movedPiece
 * @param startSquare
 * @param targetSquare
 */
void Board::movePiece(piece_t movedPiece, int startSquare, int targetSquare) {
    BitBoardUtil::toggleSquares(piecesBitBoards[movedPiece], startSquare, targetSquare);
    BitBoardUtil::toggleSquares(colorBitboards[getMoveColorIndex()], startSquare, targetSquare);

    allPieceLists[movedPiece].movePiece(startSquare, targetSquare);
    square[startSquare] = Piece::None;
    square[targetSquare] = movedPiece;
}

void Board::initialize() {
    allGameMoves.clear();
    kingSquare = {0, 0};  // Or set to invalid square as sentinel
    square.assign(64, 0); // Clear the square array

    // repititionPositionHistory = std::stack<uint64_t>();
    gameStateHistory = std::vector<GameState>();

    std::unique_ptr<GameState> currentGameState = std::make_unique<GameState>();
    plyCount = 0;

    Knights = {PieceList(10), PieceList(10)};
    Pawns = {PieceList(8), PieceList(8)};
    Rooks = {PieceList(10), PieceList(10)};
    Bishops = {PieceList(10), PieceList(10)};
    Queens = {PieceList(9), PieceList(9)};

    allPieceLists[Piece::WhitePawn] = Pawns[whiteIndex];
    allPieceLists[Piece::WhiteKnight] = Knights[whiteIndex];
    allPieceLists[Piece::WhiteBishop] = Bishops[whiteIndex];
    allPieceLists[Piece::WhiteRook] = Rooks[whiteIndex];
    allPieceLists[Piece::WhiteQueen] = Queens[whiteIndex];
    allPieceLists[Piece::WhiteKing] = PieceList(1);

    allPieceLists[Piece::BlackPawn] = Pawns[blackIndex];
    allPieceLists[Piece::BlackKnight] = Knights[blackIndex];
    allPieceLists[Piece::BlackBishop] = Bishops[blackIndex];
    allPieceLists[Piece::BlackRook] = Rooks[blackIndex];
    allPieceLists[Piece::BlackQueen] = Queens[blackIndex];
    allPieceLists[Piece::BlackKing] = PieceList(1); // Since a side will only ever have one king

    totalPieceCountWithoutPawnsAndKings = 0;

    piecesBitBoards.assign(Piece::MaxPieceIndex + 1, 0ULL);
    colorBitboards.assign(2, 0ULL);
    allPiecesBitboard = 0;

    isWhiteToMove = true;
    hasCachedInCheckValue = false;
    cachedInCheckValue = false;
}

std::string Board::createDiagram(const Board& board, bool blackAtTop = true, bool includeFen = true) {
    std::ostringstream result;

    // ANSI color codes
    const std::string RESET = "\033[0m";
    const std::string BLACK_SQUARE_BG = "\033[48;5;94m";  // Dark brown background
    const std::string WHITE_SQUARE_BG = "\033[48;5;222m"; // Light brown background
    const std::string BLACK_PIECE_FG = "\033[38;5;0m";    // Pure black text
    const std::string WHITE_PIECE_FG = "\033[38;5;231m";  // Brightest white text
    const std::string HIGHLIGHT_BG = "\033[48;5;65m";     // Muted green highlight for last move

    // Get the last move's target square for highlighting
    int lastMoveSquare = board.allGameMoves.size() > 0 ? board.allGameMoves.back().targetSquare() : -1;

    // Board frame top
    result << "  ┌────────────────────────┐\n";

    for (int y = 0; y < 8; y++) {
        int rankIndex = blackAtTop ? y : 7 - y;
        result << (rankIndex + 1) << " │"; // Rank number on the left

        for (int x = 0; x < 8; x++) {
            int fileIndex = blackAtTop ? 7 - x : x;
            int squareIndex = Board::indexFromCoord(fileIndex, rankIndex);
            bool isLightSquare = (fileIndex + rankIndex) % 2 != 0;
            int piece = board.square[squareIndex];
            bool highlight = squareIndex == lastMoveSquare;

            // Set background color based on square color and highlight status
            if (highlight) {
                result << HIGHLIGHT_BG;
            } else if (isLightSquare) {
                result << WHITE_SQUARE_BG;
            } else {
                result << BLACK_SQUARE_BG;
            }

            // Set foreground color based on piece color
            if (piece != Piece::None) {
                bool isWhitePiece = !Piece::isColor(piece, Piece::Black);
                result << (isWhitePiece ? WHITE_PIECE_FG : BLACK_PIECE_FG);
                result << " " << Piece::getPieceSymbol(piece) << " ";
            } else {
                result << "   "; // Empty square
            }

            result << RESET; // Reset all formatting
        }

        result << "│\n";
    }
    // Board frame bottom
    result << "  └────────────────────────┘\n";

    // File letters at the bottom
    result << "    ";
    for (int x = 0; x < 8; x++) {
        int fileIndex = blackAtTop ? 7 - x : x;
        result << Board::fileNames[fileIndex] << "  ";
    }
    result << "\n";

    // Include FEN string if requested
    if (includeFen) {
        result << "FEN: " << Fen::currentFen(board, true) << "\n";
    }
    result << "=============================================" << "\n";
    return result.str();
}

void Board::loadPosition(Fen::PositionInfo* posInfo) {
    startPositionInfo = posInfo;
    initialize();

    // Load pieces into board array and piece lists
    for (auto squareIndex = 0; squareIndex < 64; squareIndex++) {

        auto piece = posInfo->getSquares()[squareIndex];
        auto pieceClass = Piece::pieceClass(piece);
        auto colorIndex = Piece::isWhite(piece) ? whiteIndex : blackIndex;
        square[squareIndex] = piece;

        if (pieceClass != Piece::None) {

            BitBoardUtil::setSquare(piecesBitBoards[piece], squareIndex);
            BitBoardUtil::setSquare(colorBitboards[colorIndex], squareIndex);

            if (pieceClass == Piece::King) {
                kingSquare[colorIndex] = squareIndex;
            } else {
                allPieceLists[piece].addPieceAtSquare(squareIndex);
            }
            totalPieceCountWithoutPawnsAndKings +=
                ((pieceClass == Piece::Pawn) or (pieceClass == Piece::King));
        }
    }

    // Change side to move
    isWhiteToMove = !isWhiteToMove;

    // Set extra bitboards
    allPiecesBitboard = colorBitboards[whiteIndex] | colorBitboards[blackIndex];

    // Create GameState
    const auto whiteCastle =
        ((posInfo->whiteCastleKingside) ? 1 << 0 : 0) | ((posInfo->whiteCastleQueenside) ? 1 << 1 : 0);

    const auto blackCastle =
        ((posInfo->blackCastleKingside) ? 1 << 0 : 0) | ((posInfo->blackCastleQueenside) ? 1 << 1 : 0);

    const auto castlingRights = (posInfo->moveCount - 1) * 2 + (isWhiteToMove ? 0 : 1);

    // Set game GameState
    // currentGameState(Piece::None,)
    currentGameState =
        GameState(Piece::None, posInfo->enPassantFile, castlingRights, posInfo->fiftyMovePlyCount);

    gameStateHistory.push_back(currentGameState);
}

void Board::loadPosition(std::string fen) {
    auto posInfo = Fen::positionFromFen(fen);
    loadPosition(&posInfo);
}

void Board::loadStartPosition() { loadPosition(Fen::startPositionFen); }

Board Board::createBoard(std::string fen = Fen::startPositionFen) {
    Board board{};
    board.loadPosition(fen);
    return board;
}

Board Board::createBoard(Board source) {
    Board board{};

    for (auto move : source.allGameMoves) {
        board.makeMove(move);
    }
    return board;
}
// Helper function to print move details
void printMoveDetails(const Move& move) {
    std::cout << "Move: " << Board::squareNameFromIndex(move.StartSquare()) << " to "
              << Board::squareNameFromIndex(move.targetSquare()) << " (Raw: 0x" << std::hex
              << move.Value() << std::dec << ")"
              << "\n  Start Square: " << move.StartSquare()
              << "\n  Target Square: " << move.targetSquare() << "\n  Flag: ";
    switch (move.GetMoveFlag()) {
    case MoveFlag::NoFlag:
        std::cout << "NoFlag";
        break;
    case MoveFlag::EnPassantCaptureFlag:
        std::cout << "EnPassantCapture";
        break;
    case MoveFlag::CastleFlag:
        std::cout << "Castle";
        break;
    case MoveFlag::PawnTwoUpFlag:
        std::cout << "PawnTwoUp";
        break;
    case MoveFlag::PromoteToQueenFlag:
        std::cout << "PromoteToQueen";
        break;
    case MoveFlag::PromoteToKnightFlag:
        std::cout << "PromoteToKnight";
        break;
    case MoveFlag::PromoteToRookFlag:
        std::cout << "PromoteToRook";
        break;
    case MoveFlag::PromoteToBishopFlag:
        std::cout << "PromoteToBishop";
        break;
    default:
        std::cout << "Unknown";
        break;
    }
    std::cout << " (" << std::bitset<4>(static_cast<int>(move.GetMoveFlag())) << ")"
              << "\n  Is Promotion? " << (move.IsPromotion() ? "Yes" : "No") << "\n\n";
}

int main() {
    // Create a board with the starting position
    Board board = Board::createBoard(Fen::startPositionFen);

    std::cout << "Initial Board:\n" << static_cast<std::string>(board) << std::endl;

    // Make a few moves and print the board after each move
    Move e2e4(Board::squareIndexFromName("e2"), Board::squareIndexFromName("e4"));
    board.makeMove(e2e4, false);
    std::cout << "After e2-e4:\n" << static_cast<std::string>(board) << std::endl;

    Move g8f6(Board::squareIndexFromName("g8"), Board::squareIndexFromName("f6"));
    board.makeMove(g8f6, false);
    std::cout << "After ... Nf6:\n" << static_cast<std::string>(board) << std::endl;

    Move d2d4(Board::squareIndexFromName("d2"), Board::squareIndexFromName("d4"));
    board.makeMove(d2d4, false);
    std::cout << "After d2-d4:\n" << static_cast<std::string>(board) << std::endl;

    // Unmake the last move and print the board
    board.unmakeMove(d2d4, false);
    std::cout << "After undoing d2-d4:\n" << static_cast<std::string>(board) << std::endl;

    // Test loading a specific FEN string
    Board customBoard =
        Board::createBoard("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq c6 0 2");
    std::cout << "Custom Board (rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq c6 0 2):\n"
              << static_cast<std::string>(customBoard) << std::endl;

    // Test castling
    Board castlingBoard = Board::createBoard("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    std::cout << "Before White Kingside Castling:\n"
              << static_cast<std::string>(castlingBoard) << std::endl;
    Move whiteKingsideCastle(Board::squareIndexFromName("e1"), Board::squareIndexFromName("g1"),
                             MoveFlag::CastleFlag);
    castlingBoard.makeMove(whiteKingsideCastle, false);
    std::cout << "After White Kingside Castling:\n"
              << static_cast<std::string>(castlingBoard) << std::endl;
    castlingBoard.unmakeMove(whiteKingsideCastle, false);
    std::cout << "After undoing White Kingside Castling:\n"
              << static_cast<std::string>(castlingBoard) << std::endl;

    // Test pawn promotion
    Board promotionBoard = Board::createBoard("8/P7/8/8/8/8/8/8 w - - 0 1");
    std::cout << "Before White Pawn Promotion:\n"
              << static_cast<std::string>(promotionBoard) << std::endl;
    Move whitePromoteQueen(Board::squareIndexFromName("a7"), Board::squareIndexFromName("a8"),
                           MoveFlag::PromoteToQueenFlag);
    promotionBoard.makeMove(whitePromoteQueen, false);
    std::cout << "After White Pawn Promotion to Queen:\n"
              << static_cast<std::string>(promotionBoard) << std::endl;
    promotionBoard.unmakeMove(whitePromoteQueen, false);
    std::cout << "After undoing White Pawn Promotion:\n"
              << static_cast<std::string>(promotionBoard) << std::endl;

    // Test en passant
    Board enPassantBoard = Board::createBoard("8/8/8/p7/P7/8/8/8 w - a6 0 1");
    std::cout << "Before En Passant Capture:\n" << static_cast<std::string>(enPassantBoard) << std::endl;
    Move whiteEnPassant(Board::squareIndexFromName("a5"), Board::squareIndexFromName("b6"),
                        MoveFlag::EnPassantCaptureFlag);
    enPassantBoard.makeMove(whiteEnPassant, false);
    std::cout << "After En Passant Capture:\n" << static_cast<std::string>(enPassantBoard) << std::endl;
    enPassantBoard.unmakeMove(whiteEnPassant, false);
    std::cout << "After undoing En Passant Capture:\n"
              << static_cast<std::string>(enPassantBoard) << std::endl;

    // Test isInCheck function
    Board checkBoard1 = Board::createBoard("4k3/8/8/8/4r3/8/8/4K3 w - - 0 1");
    std::cout << "Board in check (White)? " << checkBoard1.isInCheck() << std::endl;

    Board checkBoard2 = Board::createBoard("4k3/8/8/8/4r3/8/8/4K3 b - - 0 1");
    std::cout << "Board in check (Black)? " << checkBoard2.isInCheck() << std::endl;

    return 0;
}
