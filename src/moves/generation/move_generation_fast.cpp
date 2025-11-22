#include "moves/generation/move_generation_fast.hpp"

std::vector<Move> MoveGeneratorFast::generateLegalMoves() {
    std::vector<Move> out;
    out.reserve(64);
    const AttackData attack_data = calculateAttackData(_board, _board.side);
    if (attack_data.doubleCheck) {
        generateLegalKingMoves(attack_data, out, false);
        return out;
    }
    generateLegalKingMoves(attack_data, out, false);
    generateLegalPawnMoves(attack_data, out, false);
    generateLegalKnightMoves(attack_data, out, false);
    generateLegalSlidingMoves(PieceType::Bishop, attack_data, out, false);
    generateLegalSlidingMoves(PieceType::Rook, attack_data, out, false);
    generateLegalSlidingMoves(PieceType::Queen, attack_data, out, false);
    return out;
}

std::vector<Move> MoveGeneratorFast::generateForcingMoves() {
    std::vector<Move> out;
    out.reserve(64);
    const AttackData attack_data = calculateAttackData(_board, _board.side);
    if (attack_data.doubleCheck) {
        generateLegalKingMoves(attack_data, out, true);
        return out;
    }
    generateLegalPawnMoves(attack_data, out, true);
    generateLegalKnightMoves(attack_data, out, true);
    generateLegalSlidingMoves(PieceType::Bishop, attack_data, out, true);
    generateLegalSlidingMoves(PieceType::Rook, attack_data, out, true);
    generateLegalSlidingMoves(PieceType::Queen, attack_data, out, true);
    generateLegalKingMoves(attack_data, out, true);
    return out;
}

std::vector<Move> MoveGeneratorFast::generatePseudoLegalMoves() {
    // For now, delegate to existing generator until we replace it
    MoveGenerator legacy(_board);
    return legacy.generatePseudoLegalMoves();
}

void MoveGeneratorFast::generateLegalKingMoves(const AttackData& attack_data,
                                               std::vector<Move>& out, bool forcingOnly) {
    const Side side = _board.side;
    const BitBoard friends = _board.currentState.colorBitBoards[side];
    BitBoard moves = AttackTables::kingAttacks[attack_data.kingSq.getIndex()] & ~friends;
    while (moves) {
        Square to(moves.popLSB());
        if (!attack_data.oppAttackMap.contains(to)) {
            const bool isCap = _board.currentState.colorBitBoards[!side].contains(to);
            if (!forcingOnly || isCap)
                out.emplace_back(attack_data.kingSq.getIndex(), to.getIndex());
        }
    }
    if (!forcingOnly && !attack_data.inCheck) {
        // Castling checks using Board helpers (reuse your existing rules)
        if (side == Side::White) {

            Square e1("E1"), f1("F1"), g1("G1"), d1("D1"), c1("C1"), b1("B1");
            BitBoard occ = _board.currentState.colorBitBoards[Side::White] |
                           _board.currentState.colorBitBoards[Side::Black];
            if ((_board.castlingRights & Board::whiteKingside) &&
                !_board.isSquareAttacked(e1, !side) && !_board.isSquareAttacked(f1, !side) &&
                !occ.contains(f1) && !occ.contains(g1)) {
                out.emplace_back(e1.getIndex(), g1.getIndex(), MoveFlag::CastleFlag);
            }
            if ((_board.castlingRights & Board::whiteQueenside) &&
                !_board.isSquareAttacked(e1, !side) && !_board.isSquareAttacked(d1, !side) &&
                !occ.contains(d1) && !occ.contains(c1) && !occ.contains(b1)) {
                out.emplace_back(e1.getIndex(), c1.getIndex(), MoveFlag::CastleFlag);
            }
        } else {
            Square e8("E8"), f8("F8"), g8("G8"), d8("D8"), c8("C8"), b8("B8");
            BitBoard occ = _board.currentState.colorBitBoards[Side::White] |
                           _board.currentState.colorBitBoards[Side::Black];
            if ((_board.castlingRights & Board::blackKingside) &&
                !_board.isSquareAttacked(e8, !side) && !_board.isSquareAttacked(f8, !side) &&
                !occ.contains(f8) && !occ.contains(g8)) {
                out.emplace_back(e8.getIndex(), g8.getIndex(), MoveFlag::CastleFlag);
            }
            if ((_board.castlingRights & Board::blackQueenside) &&
                !_board.isSquareAttacked(e8, !side) && !_board.isSquareAttacked(d8, !side) &&
                !occ.contains(d8) && !occ.contains(c8) && !occ.contains(b8)) {
                out.emplace_back(e8.getIndex(), c8.getIndex(), MoveFlag::CastleFlag);
            }
        }
    }
}

void MoveGeneratorFast::generateLegalKnightMoves(const AttackData& attack_data,
                                                 std::vector<Move>& out, bool forcingOnly) {
    const Side side = _board.side;
    const BitBoard friends = _board.currentState.colorBitBoards[side];
    const BitBoard enemies = _board.currentState.colorBitBoards[!side];
    BitBoard knights =
        _board.currentState.piecesBitBoards[side * 6 + static_cast<int>(PieceType::Knight)] &
        ~attack_data.pinRayMask;
    while (knights) {
        Square from(knights.popLSB());
        BitBoard targets =
            AttackTables::knightAttacks[from.getIndex()] & ~friends & attack_data.checkRayMask;
        while (targets) {
            Square to(targets.popLSB());
            const bool isCap = enemies.contains(to);
            if (!forcingOnly || isCap) out.emplace_back(from.getIndex(), to.getIndex());
        }
    }
}

void MoveGeneratorFast::generateLegalSlidingMoves(PieceType pt, const AttackData& attack_data,
                                                  std::vector<Move>& out, bool forcingOnly) {
    const Side side = _board.side;
    const BitBoard friends = _board.currentState.colorBitBoards[side];
    const BitBoard enemies = _board.currentState.colorBitBoards[!side];
    const int idx = side * 6 + static_cast<int>(pt);
    BitBoard pieces = _board.currentState.piecesBitBoards[idx];
    const BitBoard occupied = _board.currentState.colorBitBoards[Side::White] |
                              _board.currentState.colorBitBoards[Side::Black];

    while (pieces) {
        Square from(pieces.popLSB());
        // If pinned, restrict to ray through king
        BitBoard pinMask =
            attack_data.pinRayMask.contains(from)
                ? AttackTables::getRay(attack_data.kingSq.getIndex(), from.getIndex())
                : ~BitBoard(0);
        BitBoard attacks = 0;
        if (pt == PieceType::Bishop)
            attacks = AttackTables::getBishopAttacks(from.getIndex(), occupied);
        else if (pt == PieceType::Rook)
            attacks = AttackTables::getRookAttacks(from.getIndex(), occupied);
        else
            attacks = AttackTables::getBishopAttacks(from.getIndex(), occupied) |
                      AttackTables::getRookAttacks(from.getIndex(), occupied);

        BitBoard targets = attacks & ~friends & attack_data.checkRayMask & pinMask;
        while (targets) {
            Square to(targets.popLSB());
            const bool isCap = enemies.contains(to);
            if (!forcingOnly || isCap) out.emplace_back(from.getIndex(), to.getIndex());
        }
    }
}

void MoveGeneratorFast::generateLegalPawnMoves(const AttackData& attack_data,
                                               std::vector<Move>& out, bool forcingOnly) {
    const Side side = _board.side;
    const BitBoard enemies = _board.currentState.colorBitBoards[!side];
    const BitBoard allPieces = _board.currentState.colorBitBoards[Side::White] |
                               _board.currentState.colorBitBoards[Side::Black];
    BitBoard pawns =
        _board.currentState.piecesBitBoards[side * 6 + static_cast<int>(PieceType::Pawn)];

    const int promoRank = (side == Side::White) ? 7 : 0;

    while (pawns) {
        Square from(pawns.popLSB());
        const int fromIdx = from.getIndex();
        const bool isPinned = attack_data.pinRayMask.contains(from);
        const int dir = (side == Side::White) ? 8 : -8;

        // Pushes
        if (!forcingOnly) {
            int one = fromIdx + dir;
            if (one >= 0 && one < 64 && !allPieces.contains(Square(one))) {
                if (attack_data.checkRayMask.contains(Square(one))) {
                    if (one / 8 == promoRank) {
                        out.emplace_back(fromIdx, one, MoveFlag::PromoteToQueenFlag);
                        out.emplace_back(fromIdx, one, MoveFlag::PromoteToRookFlag);
                        out.emplace_back(fromIdx, one, MoveFlag::PromoteToKnightFlag);
                        out.emplace_back(fromIdx, one, MoveFlag::PromoteToBishopFlag);
                    } else {
                        out.emplace_back(fromIdx, one);
                        // double push
                        const int startRank = (side == Side::White) ? 1 : 6;
                        if (fromIdx / 8 == startRank) {
                            int two = fromIdx + 2 * dir;
                            if (!allPieces.contains(Square(two)) &&
                                attack_data.checkRayMask.contains(Square(two))) {
                                out.emplace_back(fromIdx, two, MoveFlag::PawnTwoUpFlag);
                            }
                        }
                    }
                }
            }
        }

        // Captures
        BitBoard attacks = (side == Side::White) ? AttackTables::whitePawnAttacks[fromIdx]
                                                 : AttackTables::blackPawnAttacks[fromIdx];
        BitBoard caps = attacks & enemies & attack_data.checkRayMask;
        while (caps) {
            Square to(caps.popLSB());
            if (to.getRankIndex() == promoRank) {
                out.emplace_back(fromIdx, to.getIndex(), MoveFlag::PromoteToQueenFlag);
                out.emplace_back(fromIdx, to.getIndex(), MoveFlag::PromoteToRookFlag);
                out.emplace_back(fromIdx, to.getIndex(), MoveFlag::PromoteToKnightFlag);
                out.emplace_back(fromIdx, to.getIndex(), MoveFlag::PromoteToBishopFlag);
            } else {
                out.emplace_back(fromIdx, to.getIndex());
            }
        }

        // En passant legality can be added next (simulate occupancy)
    }
}

bool MoveGeneratorFast::isMoveACheck(const Move, int) const { return false; }

