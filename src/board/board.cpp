
#include "board/board.hpp"
#include <sstream>

void Board::movePiece(Piece p, int start, int target) {
    Square from(start);
    Square to(target);

    // Clear the piece from the starting square
    currentState.piecesBitBoards[p.pieceIndex].clear(from);
    currentState.colorBitBoards[p.side].clear(from);

    // Set the piece at the target square
    currentState.piecesBitBoards[p.pieceIndex].set(to);
    currentState.colorBitBoards[p.side].set(to);
}

void Board::updateSliderBitboards() {
    for (Side side : {Side::White, Side::Black}) {
        int base = static_cast<int>(side) * 6;
        BitBoard bishops = currentState.piecesBitBoards[base + static_cast<int>(PieceType::Bishop)];
        BitBoard rooks = currentState.piecesBitBoards[base + static_cast<int>(PieceType::Rook)];
        BitBoard queens = currentState.piecesBitBoards[base + static_cast<int>(PieceType::Queen)];

        // Update diagonal sliders (bishops and queens)
        currentState.diagonalSliders[side] = bishops | queens;

        // Update orthogonal sliders (rooks and queens)
        currentState.orthoSliders[side] = rooks | queens;
    }
}

Piece Board::getPieceAt(Square s) const {
    if (!s.isValid()) {
        return Piece(PieceType::None, Side::White); // Return none for invalid squares
    }

    // Iterate over all piece bitboards (12 total: 6 per side)
    for (int i = 0; i < 12; ++i) {
        if (currentState.piecesBitBoards[i].contains(s)) {
            // Calculate side and piece type from index
            Side side = static_cast<Side>(i / 6);           // 0 for White, 1 for Black
            PieceType type = static_cast<PieceType>(i % 6); // 0=None, 1=Pawn, ..., 6=King
            return Piece(type, side);
        }
    }

    // No piece found at the square
    return Piece(PieceType::None, Side::White);
}

Board::UndoInfo Board::makeMove(Square from, Square to) {
    UndoInfo undoInfo{from,           to,           getPieceAt(from),
                      std::nullopt,   std::nullopt, enPassantSquare,
                      castlingRights, halfMoveClock};

    // Check if this is a capture move
    Piece targetPiece = getPieceAt(to);
    if (targetPiece.type != PieceType::None) {
        undoInfo.capturedPiece = targetPiece;

        // Remove captured piece from bitboards
        currentState.piecesBitBoards[targetPiece.pieceIndex].clear(to);
        currentState.colorBitBoards[targetPiece.side].clear(to);

        // Reset halfmove clock on capture
        halfMoveClock = 0;
    } else {
        // Check for en passant capture
        if (undoInfo.movedPiece.type == PieceType::Pawn && enPassantSquare.has_value() &&
            to == enPassantSquare.value()) {

            // Determine the captured pawn's square
            int capturedPawnRank = (side == Side::White) ? to.getRankIndex() - 1 : to.getRankIndex() + 1;
            Square capturedPawnSquare(to.getFile(), capturedPawnRank);

            // Store the captured pawn
            Piece capturedPawn = getPieceAt(capturedPawnSquare);
            undoInfo.capturedPiece = capturedPawn;

            // Remove the captured pawn
            currentState.piecesBitBoards[capturedPawn.pieceIndex].clear(capturedPawnSquare);
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
        int rankDiff = std::abs(to.getRankIndex() - from.getRankIndex());
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
            int fileDiff = to.getFile() - from.getFile();

            // Kingside castling
            if (fileDiff == 2) {
                // Move the rook
                Square rookFrom(7, from.getRankIndex());
                Square rookTo(5, from.getRankIndex());
                Piece rook = getPieceAt(rookFrom);
                movePiece(rook, rookFrom.getIndex(), rookTo.getIndex());
            }
            // Queenside castling
            else if (fileDiff == -2) {
                // Move the rook
                Square rookFrom(0, from.getRankIndex());
                Square rookTo(3, from.getRankIndex());
                Piece rook = getPieceAt(rookFrom);
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
            Piece captured = undoInfo.capturedPiece.value();
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
    moveHistory.push_back(undoInfo);

    return undoInfo;
}

void Board::unMakeMove(Square from, Square to, const UndoInfo& undoInfo) {
    // Move the piece back
    movePiece(undoInfo.movedPiece, to.getIndex(), from.getIndex());

    // Restore captured piece if any
    if (undoInfo.capturedPiece.has_value()) {
        Piece captured = undoInfo.capturedPiece.value();

        // Handle en passant capture specially
        if (undoInfo.movedPiece.type == PieceType::Pawn &&
            undoInfo.previousEnPassantSquare.has_value() &&
            to == undoInfo.previousEnPassantSquare.value()) {

            // Calculate the square where the captured pawn should be restored
            int capturedPawnRank = (undoInfo.movedPiece.side == Side::White) ? to.getRankIndex() - 1
                                                                             : to.getRankIndex() + 1;
            Square capturedPawnSquare(to.getFile(), capturedPawnRank);

            // Restore the captured pawn
            currentState.piecesBitBoards[captured.pieceIndex].set(capturedPawnSquare);
            currentState.colorBitBoards[captured.side].set(capturedPawnSquare);
        } else {
            // Restore normal capture
            currentState.piecesBitBoards[captured.pieceIndex].set(to);
            currentState.colorBitBoards[captured.side].set(to);
        }
    }

    // Restore castling
    if (undoInfo.movedPiece.type == PieceType::King) {
        int fileDiff = to.getFile() - from.getFile();

        // Kingside castling
        if (fileDiff == 2) {
            // Move the rook back
            Square rookFrom(7, from.getRankIndex());
            Square rookTo(5, from.getRankIndex());
            Piece rook(PieceType::Rook, undoInfo.movedPiece.side);
            movePiece(rook, rookTo.getIndex(), rookFrom.getIndex());
        }
        // Queenside castling
        else if (fileDiff == -2) {
            // Move the rook back
            Square rookFrom(0, from.getRankIndex());
            Square rookTo(3, from.getRankIndex());
            Piece rook(PieceType::Rook, undoInfo.movedPiece.side);
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
    if (!moveHistory.empty()) {
        moveHistory.pop_back();
    }
}

void Board::makeNullMove() {
    UndoInfo nullMove{Square::None,   Square::None, Piece(PieceType::None, side),
                      std::nullopt,   std::nullopt, enPassantSquare,
                      castlingRights, halfMoveClock};

    // Store the null move
    moveHistory.push_back(nullMove);

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

bool Board::calculateInCheckState() const {
    // Find the king
    Square kingSquare = Square::None;
    int kingIndex = static_cast<int>(side) * 6 + static_cast<int>(PieceType::King);
    BitBoard kingBB = currentState.piecesBitBoards[kingIndex];

    // Find the first (and only) bit set in the king bitboard
    for (int i = 0; i < 64; ++i) {
        Square sq(i);
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
    Side enemy = !side;

    // Check for pawn attacks
    int pawnAttackDir = (side == Side::White) ? 1 : -1;

    // Check diagonal left
    Square pawnCheckSquare = kingSquare.tryOffset(Offset(-1, pawnAttackDir));
    if (pawnCheckSquare != Square::None) {
        Piece piece = getPieceAt(pawnCheckSquare);
        if (piece.type == PieceType::Pawn && piece.side == enemy) {
            return true;
        }
    }

    // Check diagonal right
    pawnCheckSquare = kingSquare.tryOffset(Offset(1, pawnAttackDir));
    if (pawnCheckSquare != Square::None) {
        Piece piece = getPieceAt(pawnCheckSquare);
        if (piece.type == PieceType::Pawn && piece.side == enemy) {
            return true;
        }
    }

    // Check for knight attacks
    const Offset knightOffsets[] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                                    {1, -2},  {1, 2},  {2, -1},  {2, 1}};

    for (const auto& offset : knightOffsets) {
        Square knightCheckSquare = kingSquare.tryOffset(offset);
        if (knightCheckSquare != Square::None) {
            Piece piece = getPieceAt(knightCheckSquare);
            if (piece.type == PieceType::Knight && piece.side == enemy) {
                return true;
            }
        }
    }

    // Check for slider attacks (bishops, rooks, queens)
    // Diagonal directions for bishop/queen
    const Offset diagonalOffsets[] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

    for (const auto& offset : diagonalOffsets) {
        Square checkSquare = kingSquare;
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
        Square checkSquare = kingSquare;
        while (true) {
            checkSquare = checkSquare.tryOffset(offset);
            if (checkSquare == Square::None) break;

            Piece piece = getPieceAt(checkSquare);
            if (piece.type != PieceType::None) {
                if (Piece::isOrthoSlider(piece) && piece.side == enemy) {
                    return true;
                }
                break; // Blocked by a piece
            }
        }
    }

    // Check for king proximity (should not happen in a legal position, but checking for completeness)
    const Offset kingOffsets[] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    for (const auto& offset : kingOffsets) {
        Square kingCheckSquare = kingSquare.tryOffset(offset);
        if (kingCheckSquare != Square::None) {
            Piece piece = getPieceAt(kingCheckSquare);
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
    std::stringstream ss;

    // Print board
    for (int r = 7; r >= 0; --r) {
        if (blackAtTop) {
            ss << (r + 1) << " ";
        } else {
            ss << (8 - r) << " ";
        }

        for (int f = 0; f < 8; ++f) {
            int actualFile = blackAtTop ? 7 - f : f;
            Square sq(actualFile, r);
            Piece piece = board.getPieceAt(sq);

            // Determine background color
            bool isLightSquare = sq.isLightSquare();
            std::string bgColor = isLightSquare ? WHITE_BG : BLACK_BG;

            // Determine foreground color based on piece side
            std::string fgColor;
            if (piece.type == PieceType::None) {
                fgColor = "";
                ss << bgColor << "  " << RESET;
            } else {
                fgColor = (piece.side == Side::White) ? WHITE_FG : BLACK_FG;

                // Get piece character representation
                char pieceChar;
                switch (piece.type) {
                case PieceType::Pawn:
                    pieceChar = 'P';
                    break;
                case PieceType::Knight:
                    pieceChar = 'N';
                    break;
                case PieceType::Bishop:
                    pieceChar = 'B';
                    break;
                case PieceType::Rook:
                    pieceChar = 'R';
                    break;
                case PieceType::Queen:
                    pieceChar = 'Q';
                    break;
                case PieceType::King:
                    pieceChar = 'K';
                    break;
                default:
                    pieceChar = ' ';
                    break;
                }

                ss << bgColor << fgColor << ' ' << pieceChar << RESET;
            }
        }
        ss << std::endl;
    }

    // Print file labels
    ss << "  ";
    for (int f = 0; f < 8; ++f) {
        char fileChar = blackAtTop ? 'H' - f : 'A' + f;
        ss << ' ' << fileChar;
    }
    ss << std::endl;

    // Print additional information
    ss << "Side to move: " << (board.side == Side::White ? "White" : "Black") << std::endl;

    // Print castling rights
    ss << "Castling: ";
    if (board.castlingRights & board.whiteKingside) ss << "K";
    if (board.castlingRights & board.whiteQueenside) ss << "Q";
    if (board.castlingRights & board.blackKingside) ss << "k";
    if (board.castlingRights & board.blackQueenside) ss << "q";
    if (board.castlingRights == 0) ss << "-";
    ss << std::endl;

    // Print en passant square
    ss << "En passant: ";
    if (board.enPassantSquare.has_value()) {
        ss << board.enPassantSquare.value().getAlgebraic();
    } else {
        ss << "-";
    }
    ss << std::endl;

    // Print move clocks
    ss << "Halfmove clock: " << board.halfMoveClock << std::endl;
    ss << "Fullmove number: " << board.fullMoveClock << std::endl;

    // Print check status
    if (board.calculateInCheckState()) {
        ss << "CHECK!" << std::endl;
    }

    // Include FEN if requested
    if (includeFen) {
        // TODO: Add FEN string generation
    }

    return ss.str();
}
