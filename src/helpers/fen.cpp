#include "helpers/fen.hpp"
#include "board/board.hpp"
#include <iostream>
#include <string>

std::string Fen::currentFen(const Board& board, bool alwaysIncludeEnPassantSquare) {
    std::string fen = "";
    for (auto rank = 7; rank >= 0; rank--) {
        auto numEmptyFiles = 0;
        for (auto file = 0; file < 8; file++) {

            auto index = rank * 8 + file;
            auto piece = board.square[index];

            if (piece != Piece::None) {
                if (numEmptyFiles != 0) {
                    fen += std::to_string(numEmptyFiles);
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
            fen += std::to_string(numEmptyFiles);
        }
        if (rank != 0) {
            fen += '/';
        }
    }
    // Side to move
    fen += ' ';
    fen += (board.isWhiteToMove) ? 'w' : 'b';

    // Castling

    std::cout << "\nCastling Rights: " << board.currentGameState.castlingRights << "\n";
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
    const auto enPassantRankIndex = (board.isWhiteToMove) ? 2 : 5;
    const auto isEnPassant = enPassantFileIndex != -1;
    const auto includeEnPassant = alwaysIncludeEnPassantSquare ||
                                  enPassantCanBeCaptured(enPassantFileIndex, enPassantRankIndex, board);
    if (isEnPassant && includeEnPassant) {
        fen += Board::squareNameFromCoordinate(enPassantFileIndex, enPassantRankIndex);
    } else {
        fen += '-';
    }

    // 50 Move counter
    fen += ' ';
    fen += std::to_string(board.currentGameState.fiftyMoveCounter);

    // Full move counter
    fen += ' ';
    fen += std::to_string((board.plyCount / 2) + 1);

    return fen;
}

bool Fen::enPassantCanBeCaptured(int enPassantFileIndex, int enPassantRankIndex, Board board) {
    Coord captureFromA(enPassantFileIndex - 1, enPassantRankIndex + (board.isWhiteToMove ? -1 : 1));
    Coord captureFromB(enPassantFileIndex + 1, enPassantRankIndex + (board.isWhiteToMove ? -1 : 1));

    // FIXME heap allocate?
    auto enPassantCaptureSquare = (new Coord(enPassantFileIndex, enPassantRankIndex))->squareIndex();
    int friendlyPawn = Piece::makePiece(Piece::Pawn, board.getMoveColor());

    auto canCapture = [&](const Coord& from) -> bool {
        if (!from.isValidSquare()) {
            return false;
        }

        if (board.square[from.squareIndex()] == friendlyPawn) {
            Move move(from.squareIndex(), enPassantCaptureSquare, MoveFlag::EnPassantCaptureFlag);
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
    const auto sections = Fen::split(fen);
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
    flippedFen += " ";
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

    if (enPassant.length() > 1 && (enPassant[1] == '3' || enPassant[1] == '6')) {
        flippedEnPassant += (enPassant[1] == '6') ? '3' : '6';
    } else {
        flippedEnPassant = "-";
    }

    flippedFen += " " + flippedEnPassant;
    flippedFen += " " + sections[4] + " " + sections[5];

    return flippedFen;
}
