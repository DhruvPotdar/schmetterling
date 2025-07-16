#include "moves/generation/move_generation.hpp"
#include "board/types.hpp"
#include "moves/generation/attack_squares.hpp"
#include "moves/moves.hpp"
#include <vector>

#include <numeric>
#include <vector>

const std::vector<Move> MoveGenerator::generateMoves() {
    _legalMoves.clear();

    // Generate all pseudo-legal moves first
    auto pseudoLegalMoves = generatePseudoLegalMoves();

    // Filter out moves that leave king in check
    _legalMoves.reserve(pseudoLegalMoves.size()); // Pre-allocate

    for (const auto& move : pseudoLegalMoves) {
        if (isLegalMove(move)) {
            _legalMoves.push_back(move);
        }
    }

    return _legalMoves;
}

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

        _moveBuffer.clear();

        switch (piece.type) {
        case PieceType::Pawn:
            this->generatePawnMoves(from, _moveBuffer);
            break;
        case PieceType::King:
            this->generateKingMoves(from, _moveBuffer);
            break;
        case PieceType::Queen:
            this->generateQueenMoves(from, _moveBuffer);
            break;
        case PieceType::Knight:
            this->generateKnightMoves(from, _moveBuffer);
            break;
        case PieceType::Bishop:
            this->generateBishopMoves(from, _moveBuffer);
            break;
        case PieceType::Rook:
            this->generateRookMoves(from, _moveBuffer);
            break;
        default:
            break;
        }

        moves.insert(moves.end(), std::make_move_iterator(_moveBuffer.begin()),
                     std::make_move_iterator(_moveBuffer.end()));
    }

    return moves;
}

/**
 * @brief Makes a move on the board ,checks if the board is in check and undos
 *
 * @param move
 * @return true if not in check, false otherwise
 */
// bool MoveGenerator::isLegalMove(const Move move) const {
//     const auto undoinfo =
//         _board.makeMove(Square(move.startSquareIndex()), Square(move.targetSquareIndex()));
//     const auto legal = !_board.isInCheck();
//     _board.unMakeMove(Square(move.startSquareIndex()), Square(move.targetSquareIndex()),
//     undoinfo); return legal;
// }

bool MoveGenerator::isLegalMove(const Move move) const {
    auto _tempBoard = _board;
    _tempBoard.makeMove(Square(move.startSquareIndex()), Square(move.targetSquareIndex()));
    return !_tempBoard.isInCheck();
}

void MoveGenerator::generatePawnMoves(Square square, std::vector<Move>& moves) {
    auto piece = _board.getPieceAt(square);

    if (piece.type != PieceType::Pawn || piece.side != _board.side) {
        std::cerr << "Wrong Piece";
        return;
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
}

void MoveGenerator::generateKnightMoves(Square square, std::vector<Move>& moves) {
    const auto piece = _board.getPieceAt(square);
    if (piece.type != PieceType::Knight || piece.side != _board.side) std::cerr << "Wrong Piece";

    const auto attacks = AttackTables::knightAttacks[square.getIndex()];
    const auto ownPieces = _board.currentState.colorBitBoards[_board.side];
    auto validTargets = attacks & ~ownPieces;
    while (validTargets) {
        const auto targetSquare = validTargets.popLSB();
        moves.emplace_back(Move(square.getIndex(), targetSquare.getIndex()));
    }
}

void MoveGenerator::generateKingMoves(Square square, std::vector<Move>& moves) {
    const auto piece = _board.getPieceAt(square);
    assert(piece.type == PieceType::King && piece.side == _board.side);

    // Normal moves
    const auto attacks = AttackTables::kingAttacks[square.getIndex()];
    const auto ownPieces = _board.currentState.colorBitBoards[_board.side];
    auto validTargets = attacks & ~ownPieces;

    while (validTargets) {
        const auto targetSquare = validTargets.popLSB();
        moves.emplace_back(square.getIndex(), targetSquare.getIndex());
    }

    // Castling
    int rank = (_board.side == Side::White) ? 0 : 7;
    if (_board.side == Side::White) {
        if (_board.castlingRights & Board::whiteKingside) {
            Square f1(5, rank), g1(6, rank);
            if (_board.getPieceAt(f1).type == PieceType::None &&
                _board.getPieceAt(g1).type == PieceType::None &&
                !isSquareAttacked(square, !_board.side) && !isSquareAttacked(f1, !_board.side) &&
                !isSquareAttacked(g1, !_board.side)) {
                moves.emplace_back(square.getIndex(), g1.getIndex(), MoveFlag::CastleFlag);
            }
        }
        if (_board.castlingRights & Board::whiteQueenside) {
            Square d1(3, rank), c1(2, rank), b1(1, rank);
            if (_board.getPieceAt(d1).type == PieceType::None &&
                _board.getPieceAt(c1).type == PieceType::None &&
                _board.getPieceAt(b1).type == PieceType::None &&
                !isSquareAttacked(square, !_board.side) && !isSquareAttacked(d1, !_board.side) &&
                !isSquareAttacked(c1, !_board.side)) {
                moves.emplace_back(square.getIndex(), c1.getIndex(), MoveFlag::CastleFlag);
            }
        }
    } else {
        if (_board.castlingRights & Board::blackKingside) {
            Square f8(5, rank), g8(6, rank);
            if (_board.getPieceAt(f8).type == PieceType::None &&
                _board.getPieceAt(g8).type == PieceType::None &&
                !isSquareAttacked(square, !_board.side) && !isSquareAttacked(f8, !_board.side) &&
                !isSquareAttacked(g8, !_board.side)) {
                moves.emplace_back(square.getIndex(), g8.getIndex(), MoveFlag::CastleFlag);
            }
        }
        if (_board.castlingRights & Board::blackQueenside) {
            Square d8(3, rank), c8(2, rank), b8(1, rank);
            if (_board.getPieceAt(d8).type == PieceType::None &&
                _board.getPieceAt(c8).type == PieceType::None &&
                _board.getPieceAt(b8).type == PieceType::None &&
                !isSquareAttacked(square, !_board.side) && !isSquareAttacked(d8, !_board.side) &&
                !isSquareAttacked(c8, !_board.side)) {
                moves.emplace_back(square.getIndex(), c8.getIndex(), MoveFlag::CastleFlag);
            }
        }
    }
}
void MoveGenerator::generateBishopMoves(Square square, std::vector<Move>& moves) {
    generateSlidingMoves(square, AttackTables::bishopOffsets, 4, moves);
}

void MoveGenerator::generateRookMoves(Square square, std::vector<Move>& moves) {
    return generateSlidingMoves(square, AttackTables::rookOffsets, 4, moves);
}

void MoveGenerator::generateQueenMoves(Square square, std::vector<Move>& moves) {
    return generateSlidingMoves(square, AttackTables::queenOffsets, 8, moves);
}

void MoveGenerator::generateSlidingMoves(Square square, const Offset* directions, int numDirections,
                                         std::vector<Move>& moves) const {
    const auto piece = _board.getPieceAt(square);
    if (piece.type == PieceType::None || piece.side != _board.side) std::cerr << "Wrong Piece";

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
}

bool MoveGenerator::isSquareAttacked(Square square, Side attackerSide) const {
    const auto attackerPieces = _board.currentState.colorBitBoards[attackerSide];
    if (attackerPieces.isEmpty()) return false; // Quick exit if no attacker pieces

    // Check pawn attacks using precomputed tables
    const auto pawnAttacks = (attackerSide == Side::White)
                                 ? AttackTables::blackPawnAttacks[square.getIndex()]
                                 : AttackTables::whitePawnAttacks[square.getIndex()];
    if (pawnAttacks &
        _board.currentState.piecesBitBoards[attackerSide * 6 + static_cast<int>(PieceType::Pawn)]) {
        return true;
    }

    // Check knight attacks using precomputed tables
    if (AttackTables::knightAttacks[square.getIndex()] &
        _board.currentState
            .piecesBitBoards[attackerSide * 6 + static_cast<int>(PieceType::Knight)]) {
        return true;
    }

    // Check king attacks using precomputed tables
    if (AttackTables::kingAttacks[square.getIndex()] &
        _board.currentState.piecesBitBoards[attackerSide * 6 + static_cast<int>(PieceType::King)]) {
        return true;
    }

    // Get occupancy bitboard for blocking pieces
    const BitBoard occupancy = _board.currentState.colorBitBoards[Side::White] |
                               _board.currentState.colorBitBoards[Side::Black];

    const int squareIdx = square.getIndex();

    // Check diagonal attacks (bishops and queens)
    const auto bishopQueens =
        _board.currentState
            .piecesBitBoards[attackerSide * 6 + static_cast<int>(PieceType::Bishop)] |
        _board.currentState.piecesBitBoards[attackerSide * 6 + static_cast<int>(PieceType::Queen)];
    if (!bishopQueens.isEmpty()) {
        // Check each diagonal direction using precomputed ray masks
        for (int dir = 0; dir < 4; ++dir) { // 0-3 are diagonal directions
            BitBoard ray = AttackTables::rayMasks[squareIdx][dir];
            BitBoard blockers = ray & occupancy;

            if (!blockers.isEmpty()) {
                // Find the closest blocker in this direction
                Square blockerSq;
                if (dir < 2) { // NW, NE directions
                    blockerSq = Square(blockers.popLSB());
                } else { // SW, SE directions
                    blockerSq = Square(blockers.popMSB());
                }
                // Trim the ray at the first blocker
                ray &= ~AttackTables::rayMasks[blockerSq.getIndex()][dir];
            }

            if (ray & bishopQueens) return true;
        }
    }

    // Check orthogonal attacks (rooks and queens)
    const auto rookQueens =
        _board.currentState.piecesBitBoards[attackerSide * 6 + static_cast<int>(PieceType::Rook)] |
        _board.currentState.piecesBitBoards[attackerSide * 6 + static_cast<int>(PieceType::Queen)];
    if (!rookQueens.isEmpty()) {
        // Check each orthogonal direction using precomputed ray masks
        for (int dir = 4; dir < 8; ++dir) { // 4-7 are orthogonal directions
            BitBoard ray = AttackTables::rayMasks[squareIdx][dir];
            BitBoard blockers = ray & occupancy;

            if (!blockers.isEmpty()) {
                // Find the closest blocker in this direction
                Square blockerSq;
                if (dir == 4 || dir == 6) { // North, West directions
                    blockerSq = Square(blockers.popLSB());
                } else { // South, East directions
                    blockerSq = Square(blockers.popMSB());
                }
                // Trim the ray at the first blocker
                ray &= ~AttackTables::rayMasks[blockerSq.getIndex()][dir];
            }

            if (ray & rookQueens) return true;
        }
    }

    return false;
}

BitBoard MoveGenerator::getAttacksForPiece(Piece piece) const {
    // Implementation to be added if needed
    return BitBoard();
}
