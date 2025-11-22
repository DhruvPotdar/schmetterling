#include "moves/generation/move_generation.hpp"
#include "board/types.hpp"
#include "moves/generation/attack_squares.hpp"
#include "moves/moves.hpp"
#include <vector>

#include <algorithm>
#include <numeric>
#include <vector>

const std::vector<Move> MoveGenerator::generateMoves() {
    std::vector<Move> allMoves;
    allMoves.reserve(20); // Reserve space

    // Prepare a vector of all square indices [0, 63]
    std::vector<int> indices(64);
    std::iota(indices.begin(), indices.end(), 0);

    // Lambda to process each square
    auto processSquare = [&](int idx) {
        Square sq(idx);
        const auto piece = _board.getPieceAt(sq);
        if (piece.type == PieceType::None || piece.side != _board.side)
            return; // Skip empty or opponent's square

        std::vector<Move> moves;
        switch (piece.type) {
        case PieceType::Pawn:
            this->generatePawnMoves(sq, moves);
            break;
        case PieceType::Knight:
            this->generateKnightMoves(sq, moves);
            break;
        case PieceType::Bishop:
            this->generateBishopMoves(sq, moves);
            break;
        case PieceType::Rook:
            this->generateRookMoves(sq, moves);
            break;
        case PieceType::Queen:
            this->generateQueenMoves(sq, moves);
            break;
        case PieceType::King:
            this->generateKingMoves(sq, moves);
            break;
        default:
            break;
        }
        allMoves.insert(allMoves.end(), moves.begin(), moves.end());
    };

    // Use std::for_each to process all squares
    std::for_each(indices.begin(), indices.end(), processSquare);

    return allMoves;
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
        std::vector<Move> pieceMoves;
        switch (piece.type) {
        case PieceType::Pawn:
            generatePawnMoves(from, pieceMoves);
            break;
        case PieceType::King:
            generateKingMoves(from, pieceMoves);
            break;
        case PieceType::Queen:
            generateQueenMoves(from, pieceMoves);
            break;
        case PieceType::Knight:
            generateKnightMoves(from, pieceMoves);
            break;
        case PieceType::Bishop:
            generateBishopMoves(from, pieceMoves);
            break;
        case PieceType::Rook:
            generateRookMoves(from, pieceMoves);
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

void MoveGenerator::generatePawnMoves(Square square, std::vector<Move>& moves) {
    auto piece = _board.getPieceAt(square);

    // std::cout << piece.getPieceSymbol() << "=====+++++++++++++++++++" << piece.side << "\n";
    if (piece.type != PieceType::Pawn || piece.side != _board.side) {
        std::cerr << "Wrong Piece";
        return;
        // return moves;
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

    // return moves;
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
    // return moves;
}

void MoveGenerator::generateKingMoves(Square square, std::vector<Move>& moves) {
    const auto piece = _board.getPieceAt(square);
    if (piece.type != PieceType::King || piece.side != _board.side) std::cerr << "Wrong Piece";

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
                !_board.isSquareAttacked(square, !_board.side) &&
                !_board.isSquareAttacked(f1, !_board.side)) {
                moves.emplace_back(square.getIndex(), g1.getIndex(), MoveFlag::CastleFlag);
            }
        }
        if (_board.castlingRights & Board::whiteQueenside) {
            Square d1("D1"), c1("C1"), b1("B1");
            if (_board.getPieceAt(d1).type == PieceType::None &&
                _board.getPieceAt(c1).type == PieceType::None &&
                _board.getPieceAt(b1).type == PieceType::None &&
                !_board.isSquareAttacked(square, !_board.side) &&
                !_board.isSquareAttacked(d1, !_board.side)) {
                moves.emplace_back(square.getIndex(), c1.getIndex(), MoveFlag::CastleFlag);
            }
        }
    } else {
        if (_board.castlingRights & Board::blackKingside) {
            Square f8("F8"), g8("G8");
            if (_board.getPieceAt(f8).type == PieceType::None &&
                _board.getPieceAt(g8).type == PieceType::None &&
                !_board.isSquareAttacked(square, !_board.side) &&
                !_board.isSquareAttacked(f8, !_board.side)) {
                moves.emplace_back(square.getIndex(), g8.getIndex(), MoveFlag::CastleFlag);
            }
        }
        if (_board.castlingRights & Board::blackQueenside) {
            Square d8("D8"), c8("C8"), b8("B8");
            if (_board.getPieceAt(d8).type == PieceType::None &&
                _board.getPieceAt(c8).type == PieceType::None &&
                _board.getPieceAt(b8).type == PieceType::None &&
                !_board.isSquareAttacked(square, !_board.side) &&
                !_board.isSquareAttacked(d8, !_board.side)) {
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

BitBoard MoveGenerator::getAttacksForPiece(Piece piece) const {
    // Implementation to be added if needed
    return BitBoard();
}
