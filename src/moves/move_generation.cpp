#include "moves/move_generation.hpp"
#include "board/types.hpp"
#include "moves/attack_squares.hpp"
#include <vector>

/**
 * @brief Generates Pseudo legal moves for all pieces on the current side
 *
 * @return vector of pseudo legal moves
 */
const std::vector<Move> MoveGenerator::generatePseudoLegalMoves() {

    std::vector<Move> moves;
    auto pieces = _board.currentState.colorBitBoards[_board.side];
    while (pieces) {
        Square from(pieces.popLSB());
        const auto piece = _board.getPieceAt(from);
        std::vector<Move> pieceMoves;
        switch (piece.type) {
        case PieceType::Pawn:
            pieceMoves = generatePawnMoves(from);
            break;
        case PieceType::King:
            pieceMoves = generateKingMoves(from);
            break;
        case PieceType::Queen:
            pieceMoves = generateQueenMoves(from);
            break;
        case PieceType::Knight:
            pieceMoves = generateKnightMoves(from);
            break;
        case PieceType::Bishop:
            pieceMoves = generateBishopMoves(from);
            break;
        case PieceType::Rook:
            pieceMoves = generateRookMoves(from);
            break;
        default:
            break;
        }

        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
    }

    return moves;
}

/**
 * @brief Makes a move on a tempBoard and checks if the board is in check
 *
 * @param move
 * @return true if not in check, false otherwise
 */
bool MoveGenerator::isLegalMove(const Move move) const {
    auto _tempBoard = _board;
    _tempBoard.makeMove(Square(move.startSquareIndex()), Square(move.targetSquareIndex()));
    return !_tempBoard.isInCheck();
}

std::vector<Move> MoveGenerator::generatePawnMoves(Square square) {
    std::vector<Move> moves;
    auto piece = _board.getPieceAt(square);

    std::cout << piece.getPieceSymbol() << "=====+++++++++++++++++++" << piece.side << "\n";
    if (piece.type != PieceType::Pawn || piece.side != _board.side) {

        std::cout << Board::createDiagram(_board);
        // std::cout << piece.pieceIndex() << "++++++++";
        return moves;
    }

    const auto direction = (_board.side == Side::White) ? 1 : -1;
    const auto startRank = (_board.side == Side::White) ? 1 : 6;
    const auto promotionRank = (_board.side == Side::White) ? 7 : 0;

    const auto addPromotionMoves = [&](int fromIndex, int toIndex) {
        moves.emplace_back(fromIndex, toIndex, MoveFlag::PromoteToQueenFlag);
        moves.emplace_back(fromIndex, toIndex, MoveFlag::PromoteToRookFlag);
        moves.emplace_back(fromIndex, toIndex, MoveFlag::PromoteToKnightFlag);
        moves.emplace_back(fromIndex, toIndex, MoveFlag::PromoteToBishopFlag);
    };

    // Single Push
    const auto singlePush = square.tryOffset(Offset(0, direction));
    if (singlePush != Square::None && _board.getPieceAt(singlePush).type == PieceType::None) {
        if (singlePush.getRankIndex() == promotionRank) {
            addPromotionMoves(square.getIndex(), singlePush.getIndex());
        } else {
            moves.emplace_back(square.getIndex(), singlePush.getIndex());
            // Double Push
            if (square.getRankIndex() == startRank) {
                const auto doublePush = square.tryOffset(Offset(0, 2 * direction));
                if (doublePush != Square::None &&
                    _board.getPieceAt(doublePush).type == PieceType::None) {
                    moves.emplace_back(square.getIndex(), doublePush.getIndex(),
                                       MoveFlag::PawnTwoUpFlag);
                }
            }
        }
    }

    // Captures
    const auto attacks = (_board.side == Side::White)
                             ? AttackTables::whitePawnAttacks[square.getIndex()]
                             : AttackTables::blackPawnAttacks[square.getIndex()];
    const auto ownPieces = _board.currentState.colorBitBoards[_board.side];

    const auto occupancy = _board.currentState.colorBitBoards[Side::White] |
                           _board.currentState.colorBitBoards[Side::Black];
    auto validTargets = attacks & occupancy & ~ownPieces;

    while (validTargets) {
        const auto targetSquare = validTargets.popLSB();
        if (targetSquare.getRankIndex() == promotionRank) {
            addPromotionMoves(square.getIndex(), targetSquare.getIndex());
        } else {
            moves.emplace_back(square.getIndex(), targetSquare.getIndex());
        }
    }

    // En Passant
    if (_board.enPassantSquare.has_value()) {
        const auto enPassantSquare = _board.enPassantSquare.value();
        const auto capturingPawnRank = (_board.side == Side::White) ? 4 : 3;
        if (square.getRankIndex() == capturingPawnRank &&
            std::abs(enPassantSquare.getFile() - square.getFile()) == 1) {
            moves.emplace_back(square.getIndex(), enPassantSquare.getIndex(),
                               MoveFlag::EnPassantCaptureFlag);
        }
    }

    return moves;
}

std::vector<Move> MoveGenerator::generateKnightMoves(Square square) {
    std::vector<Move> moves;
    const auto piece = _board.getPieceAt(square);
    if (piece.type != PieceType::Knight || piece.side != _board.side) return moves;

    const auto attacks = AttackTables::knightAttacks[square.getIndex()];
    const auto ownPieces = _board.currentState.colorBitBoards[_board.side];
    auto validTargets = attacks & ~ownPieces;
    while (validTargets) {
        const auto targetSquare = validTargets.popLSB();
        moves.emplace_back(Move(square.getIndex(), targetSquare.getIndex()));
    }
    return moves;
}

std::vector<Move> MoveGenerator::generateKingMoves(Square square) {
    std::vector<Move> moves;
    const auto piece = _board.getPieceAt(square);
    if (piece.type != PieceType::King || piece.side != _board.side) return moves;

    // Normal moves
    const auto attacks = AttackTables::kingAttacks[square.getIndex()];
    const auto ownPieces = _board.currentState.colorBitBoards[_board.side];
    auto validTargets = attacks & ~ownPieces;

    while (validTargets) {
        const auto targetSquare = validTargets.popLSB();
        moves.emplace_back(square.getIndex(), targetSquare.getIndex());
    }

    // Castling
    if (_board.side == Side::White) {
        if (_board.castlingRights & Board::whiteKingside) {
            Square f1("F1"), g1("G1");
            if (_board.getPieceAt(f1).type == PieceType::None &&
                _board.getPieceAt(g1).type == PieceType::None &&
                !isSquareAttacked(square, !_board.side) && !isSquareAttacked(f1, !_board.side)) {
                moves.emplace_back(square.getIndex(), g1.getIndex(), MoveFlag::CastleFlag);
            }
        }
        if (_board.castlingRights & Board::whiteQueenside) {
            Square d1("D1"), c1("C1"), b1("B1");
            if (_board.getPieceAt(d1).type == PieceType::None &&
                _board.getPieceAt(c1).type == PieceType::None &&
                _board.getPieceAt(b1).type == PieceType::None &&
                !isSquareAttacked(square, !_board.side) && !isSquareAttacked(d1, !_board.side)) {
                moves.emplace_back(square.getIndex(), c1.getIndex(), MoveFlag::CastleFlag);
            }
        }
    } else {
        if (_board.castlingRights & Board::blackKingside) {
            Square f8("F8"), g8("G8");
            if (_board.getPieceAt(f8).type == PieceType::None &&
                _board.getPieceAt(g8).type == PieceType::None &&
                !isSquareAttacked(square, !_board.side) && !isSquareAttacked(f8, !_board.side)) {
                moves.emplace_back(square.getIndex(), g8.getIndex(), MoveFlag::CastleFlag);
            }
        }
        if (_board.castlingRights & Board::blackQueenside) {
            Square d8("D8"), c8("C8"), b8("B8");
            if (_board.getPieceAt(d8).type == PieceType::None &&
                _board.getPieceAt(c8).type == PieceType::None &&
                _board.getPieceAt(b8).type == PieceType::None &&
                !isSquareAttacked(square, !_board.side) && !isSquareAttacked(d8, !_board.side)) {
                moves.emplace_back(square.getIndex(), c8.getIndex(), MoveFlag::CastleFlag);
            }
        }
    }
    return moves;
}
std::vector<Move> MoveGenerator::generateBishopMoves(Square square) {
    return generateSlidingMoves(square, AttackTables::bishopOffsets, 4);
}

std::vector<Move> MoveGenerator::generateRookMoves(Square square) {
    return generateSlidingMoves(square, AttackTables::rookOffsets, 4);
}

std::vector<Move> MoveGenerator::generateQueenMoves(Square square) {
    return generateSlidingMoves(square, AttackTables::queenOffsets, 8);
}

std::vector<Move> MoveGenerator::generateSlidingMoves(Square square, const Offset* directions,
                                                      int numDirections) const {
    std::vector<Move> moves;
    const auto piece = _board.getPieceAt(square);
    if (piece.type == PieceType::None || piece.side != _board.side) return moves;

    const auto occupancy = _board.currentState.colorBitBoards[Side::White] |
                           _board.currentState.colorBitBoards[Side::Black];
    const auto ownPieces = _board.currentState.colorBitBoards[_board.side];
    BitBoard attacks;

    for (auto i = 0; i < numDirections; ++i) {
        auto current = square;
        while (true) {
            current = current.tryOffset(directions[i]);
            if (current == Square::None) break;
            attacks.set(current);
            if (occupancy.contains(current)) break;
        }
    }

    auto validTargets = attacks & ~ownPieces;
    while (validTargets) {
        const auto targetSquare = validTargets.popLSB();
        moves.emplace_back(square.getIndex(), targetSquare.getIndex());
    }
    return moves;
}

bool MoveGenerator::isSquareAttacked(Square square, Side attackerSide) const {

    // Pawn attacks
    const auto pawnAttacks = (attackerSide == Side::White)
                                 ? AttackTables::blackPawnAttacks[square.getIndex()]
                                 : AttackTables::whitePawnAttacks[square.getIndex()];
    if (pawnAttacks &
        _board.currentState.piecesBitBoards[attackerSide * 6 + static_cast<int>(PieceType::Pawn)]) {
        return true;
    }

    // Knight attacks
    BitBoard knightAttacks = AttackTables::knightAttacks[square.getIndex()];
    if (knightAttacks & _board.currentState.piecesBitBoards[attackerSide * 6 +
                                                            static_cast<int>(PieceType::Knight)]) {
        return true;
    }

    // King attacks
    BitBoard kingAttacks = AttackTables::kingAttacks[square.getIndex()];
    if (kingAttacks &
        _board.currentState.piecesBitBoards[attackerSide * 6 + static_cast<int>(PieceType::King)]) {
        return true;
    }

    // Sliding attacks
    BitBoard occupancy = _board.currentState.colorBitBoards[Side::White] |
                         _board.currentState.colorBitBoards[Side::Black];
    for (const auto& dir : AttackTables::bishopOffsets) {
        auto current = square;
        while (true) {
            current = current.tryOffset(dir);
            if (current == Square::None) break;
            const auto piece = _board.getPieceAt(current);
            if (piece.type != PieceType::None) {
                if (piece.side == attackerSide && Piece::isDiagonalSlider(piece)) return true;
                break;
            }
        }
    }
    for (const auto& dir : AttackTables::rookOffsets) {
        auto current = square;
        while (true) {
            current = current.tryOffset(dir);
            if (current == Square::None) break;
            const auto piece = _board.getPieceAt(current);
            if (piece.type != PieceType::None) {
                if (piece.side == attackerSide && Piece::isOrthoSlider(piece)) return true;
                break;
            }
        }
    }

    return false;
}

BitBoard MoveGenerator::getAttacksForPiece(Piece piece) const {
    // Implementation to be added if needed
    return BitBoard();
}
