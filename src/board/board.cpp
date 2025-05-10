#include "board/board.hpp"

#include "board/piece.hpp"
#include "helpers/fen.hpp"
#include "state.hpp"

#include <iostream>
#include <memory>
#include <vector>

/**
 * @brief make a move
 *
 * @param move
 */

std::string Board::getCurrentFen() { return Fen::currentFen(*this); }

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

            // TODO: Update Rook position
            //        BitBoardUtility.ToggleSquares(ref PieceBitboards[rookPiece],
            //        castlingRookFromIndex, castlingRookToIndex);
            // BitBoardUtility.ToggleSquares(ref ColourBitboards[MoveColourIndex],
            // castlingRookFromIndex, castlingRookToIndex);
            allPieceLists[rook].MovePiece(castlingRookFromIndex, castlingRookToIndex);
            square[castlingRookFromIndex] = Piece::None;
            square[castlingRookToIndex] = Piece::Rook | getMoveColor();
        }
    }

    // Handle Promotion
    if (isPromotion) {
        totalPieceCountWithoutPawnsAndKings++;

        const int promotionPieceType = [promotionPieceType, &moveFlag] {
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
        piece_t promotionPiece = Piece::makePiece(promotionPieceType, getMoveColor());

        // TODO: Remove pawn from promotion square
        // BitBoardUtility.ToggleSquare(ref PieceBitboards[movedPiece], targetSquare);
        // 				BitBoardUtility.ToggleSquare(ref PieceBitboards[promotionPiece],
        // targetSquare);

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
            while (!repititionPositionHistory.empty()) {
                repititionPositionHistory.pop();
            }
        }
        newFiftyMoveCounter = 0;
    }
    GameState newState(capturedPieceClass, newEnPassantFile, newCastlingRights,
                       newFiftyMoveCounter);
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
        // BitBoardUtility.ToggleSquare(ref PieceBitboards[promotedPiece], movedTo);
        // BitBoardUtility.ToggleSquare(ref PieceBitboards[pawnPiece], movedTo);
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
        // BitBoardUtility.ToggleSquare(ref PieceBitboards[capturedPiece], captureSquare);
        // BitBoardUtility.ToggleSquare(ref ColourBitboards[OpponentColourIndex], captureSquare);
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
            // BitBoardUtility.ToggleSquares(ref PieceBitboards[rookPiece], rookSquareAfterCastling,
            // rookSquareBeforeCastling); BitBoardUtility.ToggleSquares(ref
            // ColourBitboards[MoveColourIndex], rookSquareAfterCastling, rookSquareBeforeCastling);
            square[rookSquareAfterCastling] = Piece::None;
            square[rookSquareBeforeCastling] = rookPiece;
            allPieceLists[rookPiece].MovePiece(rookSquareAfterCastling, rookSquareBeforeCastling);
        }
    }

    allPiecesBitboard = colorBitboards[whiteIndex] | colorBitboards[blackIndex];
    updateSliderBitboards();

    if (!inSearch && repititionPositionHistory.empty()) {
        repititionPositionHistory.pop();
    }
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

// Is current player in check?
// Note: caches check value so calling multiple times does not require recalculating
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
    const auto enemyPawns = piecesBitBoards[Piece::makePiece(Piece::Pawn, getOpponentColor())];
    // const auto pawnAttackMask = isWhiteToMove ? isWhiteToMove ?
    // BitBoardUtility.WhitePawnAttacks[kingSquare] : BitBoardUtility.BlackPawnAttacks[kingSquare];
    // if ((pawnAttackMask & enemyPawns) != 0) {
    //     return true;
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
 * @brief Update piece lists / bitboards based on given move info. Note that this does not account
for the following things, which must be handled separately:
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
    // BitBoardUtility.ToggleSquares(ref PieceBitboards[piece], startSquare, targetSquare);
    // BitBoardUtility.ToggleSquares(ref ColourBitboards[MoveColourIndex], startSquare,
    // targetSquare);

    allPieceLists[movedPiece].MovePiece(startSquare, targetSquare);
    square[startSquare] = Piece::None;
    square[targetSquare] = movedPiece;
}

void Board::initialize() {
    allGameMoves.clear();
    kingSquare = {0, 0};  // Or set to invalid square as sentinel
    square.assign(64, 0); // Clear the square array

    repititionPositionHistory = std::stack<uint64_t>();
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
std::string Board::createDiagram(const Board& board, bool blackAtTop = true,
                                 bool includeFen = true) {
    // Thanks to ernestoyaquello
    std::ostringstream result;
    int lastMoveSquare =
        board.allGameMoves.size() > 0 ? board.allGameMoves.back().targetSquare() : -1;

    for (int y = 0; y < 8; y++) {
        int rankIndex = blackAtTop ? 7 - y : y;
        result << "+---+---+---+---+---+---+---+---+\n";

        for (int x = 0; x < 8; x++) {
            int fileIndex = blackAtTop ? x : 7 - x;
            int squareIndex = indexFromCoord(fileIndex, rankIndex);
            bool highlight = squareIndex == lastMoveSquare;
            int piece = board.square[squareIndex];

            if (highlight) {
                result << "|(" << Piece::getPieceSymbol(piece) << ")";
            } else {
                result << "| " << Piece::getPieceSymbol(piece) << " ";
            }

            if (x == 7) {
                // Show rank number
                result << "| " << (rankIndex + 1) << "\n";
            }
        }

        if (y == 7) {
            // Show file names
            result << "+---+---+---+---+---+---+---+---+\n";
            const std::string fileNames = "  a   b   c   d   e   f   g   h  ";
            const std::string fileNamesRev = "  h   g   f   e   d   c   b   a  ";
            result << (blackAtTop ? fileNames : fileNamesRev) << "\n\n";

            if (includeFen) {
                result << "Fen         : " << Fen::currentFen(board) << "\n";
            }
        }
    }

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

            // BitBoardUtility.SetSquare(ref PieceBitboards[piece], squareIndex);
            // BitBoardUtility.SetSquare(ref ColourBitboards[colourIndex], squareIndex);
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
    const auto whiteCastle = ((posInfo->whiteCastleKingside) ? 1 << 0 : 0) |
                             ((posInfo->whiteCastleQueenside) ? 1 << 1 : 0);

    const auto blackCastle = ((posInfo->blackCastleKingside) ? 1 << 0 : 0) |
                             ((posInfo->blackCastleQueenside) ? 1 << 1 : 0);

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

int main() { std::cout << "Hello"; }
