
#include "board/board.hpp"
#include "board/squares.hpp"
#include "board/types.hpp"
#include <iostream>
#include <sstream>
#include <string>

// Define static member
const std::string Board::startPositionFen =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void Board::movePiece(const Piece movedPiece, const int start, const int target) {
    const Square from(start);
    const Square to(target);

    // Clear the piece from the starting square

    currentState.piecesBitBoards[movedPiece.pieceIndex()].clear(from);
    currentState.colorBitBoards[movedPiece.side].clear(from);

    // Set the piece at the target square
    currentState.piecesBitBoards[movedPiece.pieceIndex()].set(to);
    currentState.colorBitBoards[movedPiece.side].set(to);
}

void Board::updateSliderBitboards() {
    for (Side side : {Side::White, Side::Black}) {
        const auto base = static_cast<int>(side) * 6;
        BitBoard bishops = currentState.piecesBitBoards[base + static_cast<int>(PieceType::Bishop)];
        BitBoard rooks = currentState.piecesBitBoards[base + static_cast<int>(PieceType::Rook)];
        BitBoard queens = currentState.piecesBitBoards[base + static_cast<int>(PieceType::Queen)];

        // Update diagonal sliders (bishops and queens)
        currentState.diagonalSliders[side] = bishops | queens;
        // Update orthogonal sliders (rooks and queens)
        currentState.orthoSliders[side] = rooks | queens;
    }
}

Piece Board::getPieceAt(const Square s) const {
    if (!s.isValid()) {
        return Piece(PieceType::None, Side::White); // Return none for invalid squares
    }

    // Iterate over all piece bitboards (12 total: 6 per side)
    for (auto i = 0; i < 12; ++i) {

        if (currentState.piecesBitBoards[i].contains(s)) {
            // Calculate side and piece type from index
            const auto side = static_cast<Side>(i / 6);        // 0 for White, 1 for Black
            const auto type = static_cast<PieceType>((i % 6)); // 0=None, 1=Pawn, ..., 6=King
            return Piece(type, side);
        }
    }

    // No piece found at the square
    return Piece(PieceType::None, Side::White);
}

Piece Board::getPieceAt(const std::string squareName) const {
    const auto s = Square(squareName);
    return getPieceAt(s);
}

Board::UndoInfo Board::makeMove(const Square from, const Square to) {
    UndoInfo undoInfo{from,           to,           getPieceAt(from),
                      std::nullopt,   std::nullopt, enPassantSquare,
                      castlingRights, halfMoveClock};

    // Check if this is a capture move
    const auto startPiece = getPieceAt(from);
    const auto targetPiece = getPieceAt(to);
    if (targetPiece.type != PieceType::None && startPiece.type != PieceType::None) {
        undoInfo.capturedPiece = targetPiece;

        // Remove captured piece from bitboards
        currentState.piecesBitBoards[targetPiece.pieceIndex()].clear(to);
        currentState.colorBitBoards[targetPiece.side].clear(to);

        // Reset halfmove clock on capture
        halfMoveClock = 0;
    } else {
        // Check for en passant capture
        if (undoInfo.movedPiece.type == PieceType::Pawn && enPassantSquare.has_value() &&
            to == enPassantSquare.value()) {

            // Determine the captured pawn's square
            const auto capturedPawnRank =
                (side == Side::White) ? to.getRankIndex() - 1 : to.getRankIndex() + 1;
            const Square capturedPawnSquare(to.getFile(), capturedPawnRank);

            // Store the captured pawn
            const auto capturedPawn = getPieceAt(capturedPawnSquare);
            undoInfo.capturedPiece = capturedPawn;

            // Remove the captured pawn

            currentState.piecesBitBoards[capturedPawn.pieceIndex()].clear(capturedPawnSquare);
            currentState.colorBitBoards[capturedPawn.side].clear(capturedPawnSquare);

            // Reset halfmove clock on capture
            halfMoveClock = 0;
        }
    }

    // Handle pawn moves
    if (undoInfo.movedPiece.type == PieceType::Pawn) {
        // Reset halfmove clock on pawn move
        halfMoveClock = 0;

        // Check for pawn double move
        const auto rankDiff = std::abs(to.getRankIndex() - from.getRankIndex());
        if (rankDiff == 2) {
            // Set en passant square
            int epRank = (side == Side::White) ? from.getRankIndex() + 1 : from.getRankIndex() - 1;
            enPassantSquare = Square(from.getFile(), epRank);
        } else {
            // Clear en passant square if not a double pawn move
            enPassantSquare = std::nullopt;
        }

        // Handle promotion (needs to be expanded based on Move class flags)
        // This part would use Move::getMoveFlag() to determine promotion piece
    } else {
        // Clear en passant square if not a pawn move
        enPassantSquare = std::nullopt;

        // Handle castling
        if (undoInfo.movedPiece.type == PieceType::King) {
            const auto fileDiff = to.getFile() - from.getFile();

            // Kingside castling
            if (fileDiff == 2) {
                // Move the rook
                Square rookFrom(7, from.getRankIndex());
                Square rookTo(5, from.getRankIndex());
                const auto rook = getPieceAt(rookFrom);
                movePiece(rook, rookFrom.getIndex(), rookTo.getIndex());
            }
            // Queenside castling
            else if (fileDiff == -2) {
                // Move the rook
                Square rookFrom(0, from.getRankIndex());
                Square rookTo(3, from.getRankIndex());
                const auto rook = getPieceAt(rookFrom);
                movePiece(rook, rookFrom.getIndex(), rookTo.getIndex());
            }

            // Update castling rights after king move
            if (side == Side::White) {
                castlingRights &= ~(whiteKingside | whiteQueenside);
            } else {
                castlingRights &= ~(blackKingside | blackQueenside);
            }
        }

        // Update castling rights after rook moves
        if (undoInfo.movedPiece.type == PieceType::Rook) {
            if (side == Side::White) {
                if (from == Square("A1")) castlingRights &= ~whiteQueenside;
                if (from == Square("H1")) castlingRights &= ~whiteKingside;
            } else {
                if (from == Square("A8")) castlingRights &= ~blackQueenside;
                if (from == Square("H8")) castlingRights &= ~blackKingside;
            }
        }

        // Update castling rights after rook captures
        if (undoInfo.capturedPiece.has_value()) {
            const auto captured = undoInfo.capturedPiece.value();
            if (captured.type == PieceType::Rook) {
                if (captured.side == Side::White) {
                    if (to == Square("A1")) castlingRights &= ~whiteQueenside;
                    if (to == Square("H1")) castlingRights &= ~whiteKingside;
                } else {
                    if (to == Square("A8")) castlingRights &= ~blackQueenside;
                    if (to == Square("H8")) castlingRights &= ~blackKingside;
                }
            }
        }

        // Increment halfmove clock for non-pawn, non-capture moves
        halfMoveClock++;
    }

    // Move the piece
    movePiece(undoInfo.movedPiece, from.getIndex(), to.getIndex());

    // Change side to move
    side = !side;

    // Increment fullmove counter after Black's move
    if (side == Side::White) {
        fullMoveClock++;
    }

    // Update slider bitboards
    updateSliderBitboards();

    // Reset check cache
    inCheckCache = false;

    // Store move in history
    undoHistory.push_back(undoInfo);

    return undoInfo;
}

void Board::unMakeMove(Square from, Square to, const UndoInfo& undoInfo) {
    // Move the piece back
    movePiece(undoInfo.movedPiece, to.getIndex(), from.getIndex());

    // Restore captured piece if any
    if (undoInfo.capturedPiece.has_value()) {
        const auto captured = undoInfo.capturedPiece.value();

        // Handle en passant capture specially
        if (undoInfo.movedPiece.type == PieceType::Pawn &&
            undoInfo.previousEnPassantSquare.has_value() &&
            to == undoInfo.previousEnPassantSquare.value()) {

            // Calculate the square where the captured pawn should be restored
            const auto capturedPawnRank = (undoInfo.movedPiece.side == Side::White)
                                              ? to.getRankIndex() - 1
                                              : to.getRankIndex() + 1;
            Square capturedPawnSquare(to.getFile(), capturedPawnRank);

            // Restore the captured pawn
            currentState.piecesBitBoards[captured.pieceIndex()].set(capturedPawnSquare);
            currentState.colorBitBoards[captured.side].set(capturedPawnSquare);
        } else {
            // Restore normal capture
            currentState.piecesBitBoards[captured.pieceIndex()].set(to);
            currentState.colorBitBoards[captured.side].set(to);
        }
    }

    // Restore castling
    if (undoInfo.movedPiece.type == PieceType::King) {
        const auto fileDiff = to.getFile() - from.getFile();

        // Kingside castling
        if (fileDiff == 2) {
            // Move the rook back
            const Square rookFrom(7, from.getRankIndex());
            const Square rookTo(5, from.getRankIndex());
            const Piece rook(PieceType::Rook, undoInfo.movedPiece.side);
            movePiece(rook, rookTo.getIndex(), rookFrom.getIndex());
        }
        // Queenside castling
        else if (fileDiff == -2) {
            // Move the rook back
            const Square rookFrom(0, from.getRankIndex());
            const Square rookTo(3, from.getRankIndex());
            const Piece rook(PieceType::Rook, undoInfo.movedPiece.side);
            movePiece(rook, rookTo.getIndex(), rookFrom.getIndex());
        }
    }

    // Restore previous state values
    enPassantSquare = undoInfo.previousEnPassantSquare;
    castlingRights = undoInfo.previousCastlingRights;
    halfMoveClock = undoInfo.previousHalfmoveClock;

    // Change side back
    side = !side;

    // Decrement fullmove counter if necessary
    if (side == Side::Black) {
        fullMoveClock--;
    }

    // Update slider bitboards
    updateSliderBitboards();

    // Reset check cache
    inCheckCache = false;

    // Remove the last entry from move history
    if (!undoHistory.empty()) {
        undoHistory.pop_back();
    }
}

void Board::makeNullMove() {
    UndoInfo nullMove{Square::None,   Square::None, Piece(PieceType::None, side),
                      std::nullopt,   std::nullopt, enPassantSquare,
                      castlingRights, halfMoveClock};

    // Store the null move
    undoHistory.push_back(nullMove);

    // Clear en passant square
    enPassantSquare = std::nullopt;

    // Switch sides
    side = !side;

    // Increment fullmove counter if necessary
    if (side == Side::White) {
        fullMoveClock++;
    }

    // Reset check cache
    inCheckCache = false;

    // Increment halfmove clock
    halfMoveClock++;
}

// TODO: Revisit one move generation is done
bool Board::calculateInCheckState() const {

    // Find the king
    auto kingSquare = Square::None;
    const auto kingIndex = static_cast<int>(side) * 6 + static_cast<int>(PieceType::King);
    const auto kingBB = currentState.piecesBitBoards[kingIndex];

    // TODO: Use pop lsb here?
    // Find the first (and only) bit set in the king bitboard
    for (auto i = 0; i < 64; ++i) {
        const Square sq(i);
        if (kingBB.contains(sq)) {
            kingSquare = sq;
            break;
        }
    }

    if (kingSquare == Square::None) {
        // King not found, shouldn't happen in a valid position
        return false;
    }

    // Enemy side
    const auto enemy = !side;

    // Check for pawn attacks
    const auto pawnAttackDir = (side == Side::White) ? 1 : -1;

    // Check diagonal left
    auto pawnCheckSquare = kingSquare.tryOffset(Offset(-1, pawnAttackDir));
    if (pawnCheckSquare != Square::None) {
        const auto piece = getPieceAt(pawnCheckSquare);
        if (piece.type == PieceType::Pawn && piece.side == enemy) {
            return true;
        }
    }

    // Check diagonal right
    pawnCheckSquare = kingSquare.tryOffset(Offset(1, pawnAttackDir));
    if (pawnCheckSquare != Square::None) {
        const auto piece = getPieceAt(pawnCheckSquare);
        if (piece.type == PieceType::Pawn && piece.side == enemy) {
            return true;
        }
    }

    // Check for knight attacks
    const Offset knightOffsets[] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                                    {1, -2},  {1, 2},  {2, -1},  {2, 1}};

    for (const auto& offset : knightOffsets) {
        const auto knightCheckSquare = kingSquare.tryOffset(offset);
        if (knightCheckSquare != Square::None) {
            const auto piece = getPieceAt(knightCheckSquare);
            if (piece.type == PieceType::Knight && piece.side == enemy) {
                return true;
            }
        }
    }

    // Check for slider attacks (bishops, rooks, queens)
    // Diagonal directions for bishop/queen
    const Offset diagonalOffsets[] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

    for (const auto& offset : diagonalOffsets) {
        auto checkSquare = kingSquare;
        while (true) {
            checkSquare = checkSquare.tryOffset(offset);
            if (checkSquare == Square::None) break;

            Piece piece = getPieceAt(checkSquare);
            if (piece.type != PieceType::None) {
                if (Piece::isDiagonalSlider(piece) && piece.side == enemy) {
                    return true;
                }
                break; // Blocked by a piece
            }
        }
    }

    // Orthogonal directions for rook/queen
    const Offset orthoOffsets[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (const auto& offset : orthoOffsets) {
        auto checkSquare = kingSquare;
        while (true) {
            checkSquare = checkSquare.tryOffset(offset);
            if (checkSquare == Square::None) break;

            const auto piece = getPieceAt(checkSquare);
            if (piece.type != PieceType::None) {
                if (Piece::isOrthoSlider(piece) && piece.side == enemy) {
                    return true;
                }
                break; // Blocked by a piece
            }
        }
    }

    // Check for king proximity (should not happen in a legal position, but checking for
    // completeness)
    const Offset kingOffsets[] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                                  {0, 1},   {1, -1}, {1, 0},  {1, 1}};

    for (const auto& offset : kingOffsets) {
        const auto kingCheckSquare = kingSquare.tryOffset(offset);
        if (kingCheckSquare != Square::None) {
            const auto piece = getPieceAt(kingCheckSquare);
            if (piece.type == PieceType::King && piece.side == enemy) {
                return true;
            }
        }
    }

    // No check detected
    return false;
}

bool Board::isInCheck() {
    // Use cached value if available
    inCheckCache = calculateInCheckState();
    return inCheckCache;
}

// Implementation of Board diagram creation

const std::string Board::createDiagram(const Board& board, const bool blackAtTop,
                                       bool const includeFen) {
    std::ostringstream ss;

    // Get the last move's target square for highlighting (if available)
    Square lastMoveSquare = board.undoHistory.size() > 0 ? board.undoHistory.back().to : -1;

    // Board frame top
    ss << "  ┌────────────────────────┐\n";

    // Print board
    for (int y = 0; y < 8; ++y) {
        const auto rankIndex = !blackAtTop ? y : 7 - y;
        ss << (rankIndex + 1) << " │"; // Rank number on the left

        for (int x = 0; x < 8; ++x) {
            const auto fileIndex = !blackAtTop ? 7 - x : x;
            Square sq(fileIndex, rankIndex);
            auto piece = board.getPieceAt(sq);
            const auto isLightSquare = (fileIndex + rankIndex) % 2 != 0;
            bool highlight = sq == lastMoveSquare;

            // Set background color based on square color and highlight status
            if (highlight) {
                ss << HIGHLIGHT_BG;
            } else if (isLightSquare) {
                ss << WHITE_BG;
            } else {
                ss << BLACK_BG;
            }

            // Set foreground color based on piece side
            if (piece.type != PieceType::None) {
                {
                    ss << (piece.side ? BLACK_FG : WHITE_FG);
                    ss << " " << piece.getPieceSymbol() << "";
                }
            } else {
                ss << "   "; // Empty square
            }
            ss << RESET; // Reset formatting
        }

        ss << "│\n"; // Right border
    }

    // Board frame bottom
    ss << "  └────────────────────────┘\n";

    // File labels at the bottom
    ss << "    ";
    for (int x = 0; x < 8; ++x) {
        const auto fileIndex = !blackAtTop ? 7 - x : x;
        ss << static_cast<char>('a' + fileIndex) << "  ";
    }
    ss << "\n";

    // Print additional information
    ss << "Side to move: " << (board.side == Side::White ? "White" : "Black") << "\n";
    ss << "Castling: ";
    if (board.castlingRights & board.whiteKingside) ss << "K";
    if (board.castlingRights & board.whiteQueenside) ss << "Q";
    if (board.castlingRights & board.blackKingside) ss << "k";
    if (board.castlingRights & board.blackQueenside) ss << "q";
    if (board.castlingRights == 0) ss << "-";
    ss << "\n";

    ss << "En passant: ";
    if (board.enPassantSquare.has_value()) {
        ss << board.enPassantSquare.value().getAlgebraic();
    } else {
        ss << "-";
    }
    ss << "\n";

    ss << "Halfmove clock: " << board.halfMoveClock << "\n";
    ss << "Fullmove number: " << board.fullMoveClock << "\n";

    // ss << static_cast<std::string>(board.currentState.colorBitBoards[1]) << "\n";
    if (board.calculateInCheckState()) {
        ss << "CHECK!\n";
    }

    // Include FEN if requested
    if (includeFen) {
        ss << "FEN: " << board.toFEN() << "\n";
    }

    ss << "=============================================\n";
    return ss.str();
}
