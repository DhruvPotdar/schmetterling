#include "moves/generation/attack_data.hpp"

// Basic, efficient attackers-to square using your existing AttackTables and slider unions.
BitBoard getAttackersTo(const Board& board, Square square, Side attackerSide, BitBoard occupied) {
    const int squareIndex = square.getIndex();
    BitBoard attackers = 0;

    // Pawns
    const auto pawnAttacks = (attackerSide == Side::White)
                                 ? AttackTables::whitePawnAttacks[squareIndex]
                                 : AttackTables::blackPawnAttacks[squareIndex];
    attackers |=
        pawnAttacks &
        board.currentState.piecesBitBoards[attackerSide * 6 + static_cast<int>(PieceType::Pawn)];

    // Knights
    attackers |=
        AttackTables::knightAttacks[squareIndex] &
        board.currentState.piecesBitBoards[attackerSide * 6 + static_cast<int>(PieceType::Knight)];

    // Kings
    attackers |=
        AttackTables::kingAttacks[squareIndex] &
        board.currentState.piecesBitBoards[attackerSide * 6 + static_cast<int>(PieceType::King)];

    // Sliders
    const BitBoard bishopAtt = AttackTables::getBishopAttacks(squareIndex, occupied);
    attackers |= bishopAtt & board.currentState.diagonalSliders[attackerSide];

    const BitBoard rookAtt = AttackTables::getRookAttacks(squareIndex, occupied);
    attackers |= rookAtt & board.currentState.orthoSliders[attackerSide];

    return attackers;
}

static inline BitBoard rayBetween(const Board& board, Square from, Square to) {
    // If you have precomputed "between" masks, use them here.
    // Placeholder: step along direction accumulating squares (not performance-critical for
    // scaffolding).
    BitBoard mask = 0;
    if (from == to) return mask;

    const auto fromFile = from.getFile(), fromRank = from.getRankIndex();
    const auto toFile = to.getFile(), toRank = to.getRankIndex();
    auto dFile = (toFile > fromFile) - (toFile < fromFile);
    auto dRank = (toRank > fromRank) - (toRank < fromRank);
    if (dFile != 0) dFile = (toFile - fromFile) / std::max(1, std::abs(toFile - fromFile));
    if (dRank != 0) dRank = (toRank - fromRank) / std::max(1, std::abs(toRank - fromRank));

    // Not aligned
    if ((dFile == 0) == (dRank == 0)) {
        // diagonal if both nonzero; orthogonal if one zero; else not aligned
    }

    int f = fromFile + dFile;
    int r = fromRank + dRank;
    while (f != toFile || r != toRank) {
        mask.set(Square(f, r));
        f += dFile;
        r += dRank;
    }
    return mask;
}

AttackData calculateAttackData(const Board& board, Side sideToMove) {
    AttackData attack_data{};

    const int kingIndex = static_cast<int>(sideToMove) * 6 + static_cast<int>(PieceType::King);
    BitBoard kingBB = board.currentState.piecesBitBoards[kingIndex];
    attack_data.kingSq = (kingBB != 0) ? Square(kingBB.LSBIndex()) : Square::None;

    const Side enemy = !sideToMove;
    const BitBoard occupied = board.currentState.colorBitBoards[Side::White] |
                              board.currentState.colorBitBoards[Side::Black];

    // Opp attack map
    attack_data.oppAttackMap = 0;
    // Pawn, knight, king, sliders for enemy
    {
        const int ksq = attack_data.kingSq.getIndex(); // may be used later
        // Build a full map by OR-ing attacks from each enemy piece type.
        // Knights/king/pawns via tables
        BitBoard enemyPawns =
            board.currentState.piecesBitBoards[enemy * 6 + static_cast<int>(PieceType::Pawn)];
        while (enemyPawns) {
            Square s(enemyPawns.popLSB());
            attack_data.oppAttackMap |= (enemy == Side::White)
                                            ? AttackTables::whitePawnAttacks[s.getIndex()]
                                            : AttackTables::blackPawnAttacks[s.getIndex()];
        }

        BitBoard enemyKnights =
            board.currentState.piecesBitBoards[enemy * 6 + static_cast<int>(PieceType::Knight)];
        while (enemyKnights) {
            Square s(enemyKnights.popLSB());
            attack_data.oppAttackMap |= AttackTables::knightAttacks[s.getIndex()];
        }

        BitBoard enemyKing =
            board.currentState.piecesBitBoards[enemy * 6 + static_cast<int>(PieceType::King)];
        if (enemyKing) attack_data.oppAttackMap |= AttackTables::kingAttacks[enemyKing.LSBIndex()];

        // Sliders
        BitBoard enemyDiag = board.currentState.diagonalSliders[enemy];
        while (enemyDiag) {
            Square s(enemyDiag.popLSB());
            attack_data.oppAttackMap |= AttackTables::getBishopAttacks(s.getIndex(), occupied);
        }

        BitBoard enemyOrtho = board.currentState.orthoSliders[enemy];
        while (enemyOrtho) {
            Square s(enemyOrtho.popLSB());
            attack_data.oppAttackMap |= AttackTables::getRookAttacks(s.getIndex(), occupied);
        }
    }

    // Checkers to our king
    BitBoard checkers = getAttackersTo(board, attack_data.kingSq, enemy, occupied);
    attack_data.inCheck = (checkers != 0);
    attack_data.doubleCheck = (checkers.moreThanOne());

    // Check ray mask
    if (!attack_data.inCheck) {
        attack_data.checkRayMask = ~BitBoard(0);
    } else if (attack_data.doubleCheck) {
        attack_data.checkRayMask = 0; // only king moves are allowed
    } else {
        // Single checker
        Square checkerSq(checkers.LSBIndex());
        const auto checkerPiece = board.getPieceAt(checkerSq);
        if (checkerPiece.type == PieceType::Knight || checkerPiece.type == PieceType::Pawn) {
            attack_data.checkRayMask = BitBoard().setReturn(checkerSq);
        } else {
            attack_data.checkRayMask =
                rayBetween(board, attack_data.kingSq.getIndex(), checkerSq.getIndex());
            attack_data.checkRayMask.set(checkerSq);
        }
    }

    // Pin detection (mark pinned friendly pieces)
    attack_data.pinRayMask = 0;
    // For each friendly piece aligned with king, see if removing it reveals enemy slider
    const BitBoard friends = board.currentState.colorBitBoards[sideToMove];
    BitBoard aligned = friends; // simple pass; could be optimized using rays
    while (aligned) {
        Square s(aligned.popLSB());
        // Skip king itself
        if (s == attack_data.kingSq) continue;
        // Only consider squares aligned with king on a slider line
        // Quick test: get rook/bishop rays from king and see if it contains s
        const BitBoard rookRay =
            AttackTables::getRookAttacks(attack_data.kingSq.getIndex(), occupied);
        const BitBoard bishopRay =
            AttackTables::getBishopAttacks(attack_data.kingSq.getIndex(), occupied);
        const bool onOrtho = rookRay.contains(s);
        const bool onDiag = bishopRay.contains(s);
        if (!onOrtho && !onDiag) continue;

        // Remove this piece and test discovered attack
        BitBoard occWithout = occupied;
        occWithout.clear(s);
        bool pinned = false;
        if (onOrtho) {
            BitBoard rookAtt =
                AttackTables::getRookAttacks(attack_data.kingSq.getIndex(), occWithout);
            pinned |= (rookAtt & board.currentState.orthoSliders[enemy]);
        }
        if (onDiag) {
            BitBoard bishAtt =
                AttackTables::getBishopAttacks(attack_data.kingSq.getIndex(), occWithout);
            pinned |= (bishAtt & board.currentState.diagonalSliders[enemy]);
        }
        if (pinned) attack_data.pinRayMask.set(s);
    }

    return attack_data;
}

