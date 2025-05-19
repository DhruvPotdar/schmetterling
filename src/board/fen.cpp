#include "board/fen.hpp"
#include "board/board.hpp"
#include <sstream>
#include <stdexcept>

void FEN::parse(const std::string& fen, Board& board) {
    board.currentState.colorBitBoards.fill(BitBoard());
    board.currentState.piecesBitBoards.fill(0);
    board.side = Side::White;
    board.castlingRights = 0;
    board.enPassantSquare = std::nullopt;
    board.halfMoveClock = 0;
    board.fullMoveClock = 1;
    board.inCheckCache = false;

    std::istringstream fenStream(fen);
    std::string boardPart, activeColor, castling, enPassant, halfMove, fullMove;

    fenStream >> boardPart >> activeColor >> castling >> enPassant >> halfMove >> fullMove;

    parsePiecePlacement(boardPart, board);
    board.side = parseActiveColor(activeColor);
    board.castlingRights = parseCastlingRights(castling);
    board.enPassantSquare = parseEnPassant(enPassant);
    board.halfMoveClock = parseHalfMoveClock(halfMove);
    board.fullMoveClock = parseFullMoveNumber(fullMove);

    board.updateSliderBitboards();
}

std::string FEN::generate(const Board& board) {
    std::string fen;
    fen += generatePiecePlacement(board) + " ";
    fen += generateActiveColor(board) + " ";
    fen += generateCastlingRights(board) + " ";
    fen += generateEnPassant(board) + " ";
    fen += generateHalfMoveClock(board) + " ";
    fen += generateFullMoveNumber(board);
    return fen;
}

void FEN::parsePiecePlacement(const std::string& placement, Board& board) {
    auto file = 0;
    auto rank = 7;

    for (char c : placement) {
        if (c == '/') {
            file = 0;
            rank--;
        } else if (isdigit(c)) {
            file += c - '0';
        } else {
            Square sq(file, rank);
            Piece piece(c);
            board.currentState.piecesBitBoards[piece.pieceIndex].set(sq);
            board.currentState.colorBitBoards[piece.side].set(sq);
            file++;
        }
    }
}

Side FEN::parseActiveColor(const std::string& color) {
    if (color == "w") return Side::White;
    if (color == "b") return Side::Black;
    throw std::invalid_argument("Invalid active color in FEN");
}

uint8_t FEN::parseCastlingRights(const std::string& castling) {
    auto rights = 0;
    if (castling == "-") return rights;
    for (char c : castling) {
        switch (c) {
        case 'K':
            rights |= Board::whiteKingside;
            break;
        case 'Q':
            rights |= Board::whiteQueenside;
            break;
        case 'k':
            rights |= Board::blackKingside;
            break;
        case 'q':
            rights |= Board::blackQueenside;
            break;
        default:
            throw std::invalid_argument("Invalid castling rights in FEN");
        }
    }
    return rights;
}

std::optional<Square> FEN::parseEnPassant(const std::string& enPassant) {
    if (enPassant == "-") return std::nullopt;
    return Square(enPassant);
}

int FEN::parseHalfMoveClock(const std::string& halfMove) { return std::stoi(halfMove); }

int FEN::parseFullMoveNumber(const std::string& fullMove) { return std::stoi(fullMove); }

std::string FEN::generatePiecePlacement(const Board& board) {
    std::string placement;

    for (auto r = 7; r >= 0; --r) {
        auto emptyCount = 0;

        for (int f = 0; f < 8; ++f) {
            Square sq(f, r);
            const auto piece = board.getPieceAt(sq);
            if (piece.type == PieceType::None) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    placement += std::to_string(emptyCount);
                    emptyCount = 0;
                }
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
                if (piece.side == Side::Black) {
                    pieceChar = tolower(pieceChar);
                }
                placement += pieceChar;
            }
        }
        if (emptyCount > 0) {
            placement += std::to_string(emptyCount);
        }
        if (r > 0) {
            placement += '/';
        }
    }
    return placement;
}

std::string FEN::generateActiveColor(const Board& board) {
    return (board.side == Side::White) ? "w" : "b";
}

std::string FEN::generateCastlingRights(const Board& board) {
    std::string castling;
    if (board.castlingRights & Board::whiteKingside) castling += 'K';
    if (board.castlingRights & Board::whiteQueenside) castling += 'Q';
    if (board.castlingRights & Board::blackKingside) castling += 'k';
    if (board.castlingRights & Board::blackQueenside) castling += 'q';
    if (castling.empty()) castling = "-";
    return castling;
}

std::string FEN::generateEnPassant(const Board& board) {
    if (board.enPassantSquare.has_value()) {
        return board.enPassantSquare.value().getAlgebraic();
    }
    return "-";
}

std::string FEN::generateHalfMoveClock(const Board& board) {
    return std::to_string(board.halfMoveClock);
}

std::string FEN::generateFullMoveNumber(const Board& board) {
    return std::to_string(board.fullMoveClock);
}
