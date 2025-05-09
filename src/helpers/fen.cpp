#include "helpers/fen.hpp"
#include "board/board.hpp"

std::string Fen::currentFen(const Board board, bool alwaysIncludeEnPassantSquare) {
    std::string fen = "";
    for (auto rank = 7; rank > 0; rank--) {
        auto numEmptyFiles = 0;
        for (auto file = 0; file < 8; file++) {
            auto index = rank * 8 + file;
            auto piece = board.square[index];
            if (piece != Piece::None) {
                if (numEmptyFiles != 0) {
                    fen += numEmptyFiles;
                    numEmptyFiles = 0;
                }
                const auto isBlack = Piece::isColor(piece, Piece::Black);
                const auto pieceClass = Piece::pieceClass(piece);
                char pieceChar = ' ';
                switch (pieceClass) {
                case Piece::Rook:
                    pieceChar = 'R';
                    break;
                case Piece::Knight:
                    pieceChar = 'N';
                    break;
                case Piece::Bishop:
                    pieceChar = 'B';
                    break;
                case Piece::Queen:
                    pieceChar = 'Q';
                    break;
                case Piece::King:
                    pieceChar = 'K';
                    break;
                case Piece::Pawn:
                    pieceChar = 'P';
                    break;
                }
                fen += isBlack ? std::tolower(pieceChar) : pieceChar;
            } else {
                numEmptyFiles++;
            }
        }
        if (numEmptyFiles != 0) {
            fen += numEmptyFiles;
        }
        if (rank != 0) {
            fen += '/';
        }
    }
    // Side to move
    fen += ' ';
    fen += (board.isWhiteToMove) ? 'w' : 'b';

    // Castling

    const auto whiteKingside = (board.currentGameState.castlingRights & 1) == 1;
    const auto whiteQueenside = (board.currentGameState.castlingRights >> 1 & 1) == 1;
    const auto blackKingside = (board.currentGameState.castlingRights >> 2 & 1) == 1;
    const auto blackQueenside = (board.currentGameState.castlingRights >> 3 & 1) == 1;
    fen += ' ';
    fen += (whiteKingside) ? "K" : "";
    fen += (whiteQueenside) ? "Q" : "";
    fen += (blackKingside) ? "k" : "";
    fen += (blackQueenside) ? "q" : "";
    fen += ((board.currentGameState.castlingRights) == 0) ? "-" : "";

    // En passant
    fen += ' ';
    const auto enPassantFileIndex = board.currentGameState.enPassantFile - 1;
    const auto enPassantRankIndex = (board.isWhiteToMove) ? 5 : 2;
    const auto isEnPassant = enPassantFileIndex != -1;
    const auto includeEnPassant =
        alwaysIncludeEnPassantSquare ||
        enPassantCanBeCaptured(enPassantFileIndex, enPassantRankIndex, board);
    if (isEnPassant && includeEnPassant) {
        fen += Board::squareNameFromCoordinate(enPassantFileIndex, enPassantRankIndex);
    } else {
        fen += '-';
    }

    // 50 Move counter
    fen += ' ';
    fen += board.currentGameState.fiftyMoveCounter;

    // Full move counter
    fen += ' ';
    fen += (board.plyCount / 2) + 1;

    return fen;
}

bool Fen::enPassantCanBeCaptured(int enPassantFileIndex, int enPassantRankIndex, Board board) {
    Coord captureFromA(enPassantFileIndex - 1, enPassantRankIndex + (board.isWhiteToMove ? -1 : 1));
    Coord captureFromB(enPassantFileIndex + 1, enPassantRankIndex + (board.isWhiteToMove ? -1 : 1));

    auto enPassantCaptureSquare =
        (new Coord(enPassantFileIndex, enPassantRankIndex))->SquareIndex();
    int friendlyPawn = Piece::makePiece(Piece::Pawn, board.getMoveColor());

    auto canCapture = [&](const Coord& from) -> bool {
        if (!from.IsValidSquare()) {
            return false;
        }

        if (board.square[from.SquareIndex()] == friendlyPawn) {
            Move move(from.SquareIndex(), enPassantCaptureSquare, MoveFlag::EnPassantCaptureFlag);
            board.makeMove(move, false);
            board.makeNullMove();
            const auto wasLegalMove = !board.calculateInCheckState();
            board.unmakeNullMove();
            board.unmakeMove(move, false);
            return wasLegalMove;
        }
        return false;
    };
    return canCapture(captureFromA) || canCapture(captureFromB);
}
std::string Fen::flipFen(std::string fen) {
    std::string flippedFen = "";
    std::vector<std::string> sections;
    std::vector<char> invertedFenChars;
    const auto fenRanks = Fen::split(sections[0], '/');

    // Helper
    const auto invertCase = [](char c) -> char {
        if (std::islower(c)) {
            return std::toupper(c);
        }
        return std::tolower(c);
    };

    for (auto i = fenRanks.size() - 1; i >= 0; i--) {
        const auto rank = fenRanks[i];
        for (char c : rank) {
            flippedFen += invertCase(c);
        }
        if (i != 0) {
            flippedFen += "/";
        }
    }
    flippedFen += (sections[1][0] == 'w' ? "b" : "w");
    const std::string castlingRights = sections[2];
    std::string flippedRights = "";

    for (char c : "kqKQ") {
        if (castlingRights.contains(c)) {
            flippedRights += invertCase(c);
        }
    }
    flippedFen += " " + (flippedRights.size() == 0 ? "-" : flippedRights);

    const std::string enPassant = sections[3];
    std::string flippedEnPassant(1, enPassant[0]);

    if (enPassant.length() > 1) {
        flippedEnPassant += (enPassant[1] == '6') ? '3' : '6';
    }

    flippedFen += " " + flippedEnPassant;
    flippedFen += " " + sections[4] + " " + sections[5];

    return flippedFen;
}
