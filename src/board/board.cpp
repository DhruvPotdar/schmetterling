#include "board/board.hpp"
#include "board/helper.hpp"
#include <iostream>
#include <vector>

/**
 * @brief make a move
 *
 * @param move
 */
void Board::makeMove(Move move) {

    // Get info about the move
    auto startSquare = move.StartSquare();
    auto targetSquare = move.TargetSquare();
    auto moveFlag = move.GetMoveFlag();
    auto isPromotion = move.IsPromotion();
    bool isEnPassant = moveFlag == MoveFlag::EnPassantCaptureFlag;

    auto movedPiece = square[startSquare];
    auto movedPieceClass = Piece::pieceClass(movedPiece);
    auto capturedPiece =
        isEnPassant ? Piece::makePiece(Piece::Pawn, getOpponentColor()) : square[targetSquare];
    auto capturedPieceClass = Piece::pieceClass(capturedPiece);

    // Castling and EnPassant
    int prevCastleState = currentGameState.castlingRights;
    int prevEnPassantFile = currentGameState.enPassantFile;
    int newCastlingRights = currentGameState.castlingRights;
    int newEnPassantFile = 0;

    Board::movePiece(movedPiece, startSquare, targetSquare);

    // Captures
    if (capturedPieceClass != Piece::None) {
        auto captureSquare = targetSquare;
        if (isEnPassant) {
            captureSquare = targetSquare + (IsWhiteToMove ? -8 : 8);
            square[captureSquare] = Piece::None;
        }
        if (capturedPieceClass != Piece::Pawn) {
            totalPieceCountWithoutPawnsAndKings--;
        }

        // Remove Captured piece from bitboard and piece piecelist
        allPieceLists[capturedPiece].RemovePieceAtSquare(captureSquare);
    }

    // Handle Kings
    if (movedPieceClass == Piece::King) {
        kingSquare[getMoveColorIndex()] = targetSquare;
        newCastlingRights &= (IsWhiteToMove) ? 0b1100 : 0b0011;

        // Handle castling
        if (moveFlag == MoveFlag::CastleFlag) {
            piece_t rook = Piece::makePiece(Piece::Rook, getMoveColor());
            bool kingSide = targetSquare == BoardHelper::g1 || targetSquare == BoardHelper::g8;

            int castlingRookFromIndex = (kingSide) ? targetSquare + 1 : targetSquare - 2;
            int castlingRookToIndex = (kingSide) ? targetSquare - 1 : targetSquare + 1;

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

        int promotionPieceType;
        switch (moveFlag) {
        case MoveFlag::PromoteToQueenFlag:
            promotionPieceType = Piece::Queen;
            break;
        case MoveFlag::PromoteToRookFlag:
            promotionPieceType = Piece::Rook;
            break;
        case MoveFlag::PromoteToKnightFlag:
            promotionPieceType = Piece::Knight;
            break;
        case MoveFlag::PromoteToBishopFlag:
            promotionPieceType = Piece::Bishop;
            break;
        default:
            promotionPieceType = Piece::None;
            break;
        }
        piece_t promotionPiece = Piece::makePiece(promotionPieceType, getMoveColor());

        // TODO: Remove pawn from promotion square
        // BitBoardUtility.ToggleSquare(ref PieceBitboards[movedPiece], targetSquare);
        // 				BitBoardUtility.ToggleSquare(ref PieceBitboards[promotionPiece],
        // targetSquare);

        allPieceLists[movedPiece].RemovePieceAtSquare(targetSquare);
        allPieceLists[promotionPiece].AddPieceAtSquare(targetSquare);
        square[targetSquare] = promotionPiece;
    }
    // Pawn has moved two forwards, mark file with en-passant flag
    if (moveFlag == MoveFlag::PawnTwoUpFlag) {
        int file = BoardHelper::FileIndex(startSquare) + 1;
        newEnPassantFile = file;
    }

    // Update Castling rights
    if (prevCastleState != 0) {
        if (targetSquare == BoardHelper::h1 || startSquare == BoardHelper::h1) {
            newCastlingRights &= GameState::ClearWhiteKingsideMask;
        } else if (targetSquare == BoardHelper::a1 || startSquare == BoardHelper::a1) {
            newCastlingRights &= GameState::ClearWhiteQueensideMask;
        }
        if (targetSquare == BoardHelper::h8 || startSquare == BoardHelper::h8) {
            newCastlingRights &= GameState::ClearBlackKingsideMask;
        } else if (targetSquare == BoardHelper::a8 || startSquare == BoardHelper::a8) {
            newCastlingRights &= GameState::ClearBlackQueensideMask;
        }
    }

    // Change Side to move
    IsWhiteToMove = !IsWhiteToMove;

    plyCount++;
    int newFiftyMoveCounter = currentGameState.fiftyMoveCounter + 1;

    // Update extra bitboards
    allPiecesBitboard = colourBitboards[whiteIndex] | colourBitboards[blackIndex];
    updateSliderBitboards();
}

void Board::initialize() { square.clear(); }
int main() { std::cout << "Hello"; }
