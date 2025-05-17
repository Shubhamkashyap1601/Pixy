#include "board.h"
#include "pawn.h"
#include "rook.h"
#include "knight.h"
#include "bishop.h"
#include "queen.h"
#include "king.h"
#include "move.h"
#include <iostream>
#include <string>
#include <sstream>

Board::Board() : lastMove(0, 0, 0, 0, nullptr) {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            board[r][c] = nullptr;
    currentTurn = PieceColor::WHITE;
}

Board::~Board() {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            // if (board[r][c] != nullptr) {
            //     std::cout << "Deleting piece at [" << r << "][" << c << "]" 
            //               << " at address: " << board[r][c] << "\n";
                delete board[r][c];
                board[r][c] = nullptr;
            // } else {
            //     std::cout << "No piece at [" << r << "][" << c << "]\n";
            // }
        }
    }
}




void Board::setupBoard() {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            delete board[r][c];
            board[r][c] = nullptr;
        }

    for (int i = 0; i < 8; ++i) {
        board[1][i] = new Pawn(PieceColor::BLACK);
        board[6][i] = new Pawn(PieceColor::WHITE);
    }

    board[0][0] = board[0][7] = new Rook(PieceColor::BLACK);
    board[7][0] = board[7][7] = new Rook(PieceColor::WHITE);

    board[0][1] = board[0][6] = new Knight(PieceColor::BLACK);
    board[7][1] = board[7][6] = new Knight(PieceColor::WHITE);

    board[0][2] = board[0][5] = new Bishop(PieceColor::BLACK);
    board[7][2] = board[7][5] = new Bishop(PieceColor::WHITE);

    board[0][3] = new Queen(PieceColor::BLACK);
    board[7][3] = new Queen(PieceColor::WHITE);

    board[0][4] = new King(PieceColor::BLACK);
    board[7][4] = new King(PieceColor::WHITE);
}

void Board::printBoard() const {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (board[r][c]) {
                char ch = ' ';
                switch (board[r][c]->getType()) {
                    case PieceType::PAWN: ch = 'P'; break;
                    case PieceType::ROOK: ch = 'R'; break;
                    case PieceType::KNIGHT: ch = 'N'; break;
                    case PieceType::BISHOP: ch = 'B'; break;
                    case PieceType::QUEEN: ch = 'Q'; break;
                    case PieceType::KING: ch = 'K'; break;
                    default: break;
                }

                if (board[r][c]->getColor() == PieceColor::BLACK)
                    ch = std::tolower(ch);
                std::cout << ch << ' ';
            } else {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
}

bool Board::movePiece(int fromRow, int fromCol, int toRow, int toCol) {

    Piece* piece = board[fromRow][fromCol];
    if (!piece) {
        std::cout << "Invalid move (no piece at source)." << std::endl;
        return false;
    }

    if (piece->getColor() != currentTurn) {
        std::cout << "It's not " << (currentTurn == PieceColor::WHITE ? "White" : "Black") << "'s turn!\n";
        return false;
    }

    bool isEnPassant = false;
    if (piece->getType() == PieceType::PAWN) {
        Pawn* pawn = static_cast<Pawn*>(piece);
        if (lastMove.pieceMoved && pawn->canEnPassant(fromRow, fromCol, toRow, toCol, board, lastMove)) {
            isEnPassant = true;
        }
    }


    if (piece->getType() == PieceType::KING && std::abs(toCol - fromCol) == 2 && fromRow == toRow) {
        bool isKingside = toCol > fromCol;
        int row = fromRow;
        int rookCol = isKingside ? 7 : 0;
        int rookDestCol = isKingside ? 5 : 3;
        int step = isKingside ? 1 : -1;

        for (int c = fromCol + step; c != rookCol; c += step)
            if (board[row][c]) {
                std::cout << "Castling blocked: path not clear.\n";
                return false;
            }

        if (isInCheck(piece->getColor())) {
            std::cout << "Castling invalid: King is currently in check.\n";
            return false;
        }

        Piece* origKing = board[row][fromCol];
        for (int i = 1; i <= 2; ++i) {
            int c = fromCol + step * i;
            Piece* origDest = board[row][c];

            board[row][fromCol] = nullptr;
            board[row][c] = origKing;

            if (isInCheck(piece->getColor())) {
                board[row][c] = origDest;
                board[row][fromCol] = origKing;
                std::cout << "Castling invalid: King would pass through check.\n";
                return false;
            }

            board[row][c] = origDest;
            board[row][fromCol] = origKing;
        }

        board[row][toCol] = piece;
        board[row][fromCol] = nullptr;

        Piece* rook = board[row][rookCol];
        board[row][rookDestCol] = rook;
        board[row][rookCol] = nullptr;

        lastMove = Move(fromRow, fromCol, toRow, toCol, piece);

        std::cout << "Castling executed!\n";
        return true;
    }

    if (!isEnPassant && !piece->isValidMove(fromRow, fromCol, toRow, toCol, board)) {
        std::cout << "Invalid move for this piece!" << std::endl;
        return false;
    }


    Piece* captured = board[toRow][toCol];

    if (isEnPassant) {
        int capturedRow = fromRow;
        int capturedCol = toCol;
        captured = board[capturedRow][capturedCol];
        board[capturedRow][capturedCol] = nullptr;
        std::cout << "En passant captured!\n";
    }

    board[toRow][toCol] = piece;
    board[fromRow][fromCol] = nullptr;

    if (isInCheck(piece->getColor())) {
        board[fromRow][fromCol] = piece;
        board[toRow][toCol] = captured;

        if (isEnPassant) {
            int capturedRow = fromRow;
            int capturedCol = toCol;
            board[capturedRow][capturedCol] = captured;
        }

        std::cout << "Move puts king in check! Invalid.\n";
        return false;
    }

    if (captured) delete captured;

    if (piece->getType() == PieceType::PAWN) {
        if ((piece->getColor() == PieceColor::WHITE && toRow == 0) ||
            (piece->getColor() == PieceColor::BLACK && toRow == 7)) {
            
            char choice;
            std::cout << "Promote pawn to (Q, R, B, N): ";
            std::cin >> choice;
            choice = std::toupper(choice);

            delete piece;

            switch (choice) {
                case 'Q': piece = new Queen(board[toRow][toCol]->getColor()); break;
                case 'R': piece = new Rook(board[toRow][toCol]->getColor()); break;
                case 'B': piece = new Bishop(board[toRow][toCol]->getColor()); break;
                case 'N': piece = new Knight(board[toRow][toCol]->getColor()); break;
                default:
                    std::cout << "Invalid choice, promoting to Queen by default.\n";
                    piece = new Queen(board[toRow][toCol]->getColor());
                    break;
            }

            board[toRow][toCol] = piece;
        }
    }

    lastMove = Move(fromRow, fromCol, toRow, toCol, piece);
    recordPosition();
    if (piece->getType() == PieceType::KING) {
        if (piece->getColor() == PieceColor::WHITE) hasWhiteKingMoved = true;
        else hasBlackKingMoved = true;
    }

    if (piece->getType() == PieceType::ROOK) {
        if (piece->getColor() == PieceColor::WHITE) {
            if (fromRow == 7 && fromCol == 0) hasWhiteQueensideRookMoved = true;
            else if (fromRow == 7 && fromCol == 7) hasWhiteKingsideRookMoved = true;
        } else {
            if (fromRow == 0 && fromCol == 0) hasBlackQueensideRookMoved = true;
            else if (fromRow == 0 && fromCol == 7) hasBlackKingsideRookMoved = true;
        }
    }

    // Update halfmoveClock for 50-move rule
    if (piece->getType() == PieceType::PAWN || captured != nullptr) {
        halfmoveClock = 0;
    } else {
        ++halfmoveClock;
    }
    currentTurn = (currentTurn == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;

    return true;
}


Move Board::getLastMove() const {
    return lastMove;
}

Piece* Board::getPiece(int row, int col) {
    return board[row][col];
}

Piece* const (*Board::getBoard() const)[8] {
    return board;
}

bool Board::isInCheck(PieceColor kingColor) const {
    int kingRow = -1, kingCol = -1;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (board[row][col] && 
                board[row][col]->getType() == PieceType::KING &&
                board[row][col]->getColor() == kingColor) {
                kingRow = row;
                kingCol = col;
                goto foundKing;
            }
        }
    }
    foundKing:
    if (kingRow == -1) return false;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* p = board[row][col];
            if (p && p->getColor() != kingColor) {
                if (p->isValidMove(row, col, kingRow, kingCol, const_cast<Piece* (*)[8]>(board))) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Board::isInCheckAfterMove(int fromRow, int fromCol, int toRow, int toCol, PieceColor kingColor) const {
    Board tempBoard(*this); 
    Piece* movingPiece = tempBoard.board[fromRow][fromCol];

    if (!movingPiece) return false;

    Piece* captured = tempBoard.board[toRow][toCol];
    tempBoard.board[toRow][toCol] = movingPiece;
    tempBoard.board[fromRow][fromCol] = nullptr;
    delete captured;

    return tempBoard.isInCheck(kingColor);
}


PieceColor Board::getCurrentTurn() const {
    return currentTurn;
}

Piece *(*Board::getWritableBoard())[8]
{
    return board;
}

Board::Board(const Board& other) : lastMove(other.lastMove) {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = other.board[row][col];
            board[row][col] = piece ? piece->clone() : nullptr;
        }
    }
    currentTurn = other.currentTurn;
}

Board& Board::operator=(const Board& other) {
    if (this == &other) return *this;

    // Clean up existing pieces
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            delete board[row][col];
            board[row][col] = nullptr;
        }
    }

    // Deep copy
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = other.board[row][col];
            board[row][col] = piece ? piece->clone() : nullptr;
        }
    }

    currentTurn = other.currentTurn;
    return *this;
}

void Board::loadFEN(const std::string& fen) {
    std::istringstream ss(fen);
    std::string boardPart, activeColor, castling, enPassant;
    std::string halfmoveClockStr, fullmoveNumberStr;

    ss >> boardPart >> activeColor >> castling >> enPassant >> halfmoveClockStr >> fullmoveNumberStr;
    halfmoveClock = std::stoi(halfmoveClockStr);

    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            delete board[r][c];
            board[r][c] = nullptr;
        }

    int row = 0, col = 0;
    for (char ch : boardPart) {
        if (ch == '/') {
            row++;
            col = 0;
        } else if (std::isdigit(ch)) {
            col += ch - '0';
        } else {
            PieceColor color = std::isupper(ch) ? PieceColor::WHITE : PieceColor::BLACK;
            char typeChar = std::tolower(ch);

            Piece* piece = nullptr;
            switch (typeChar) {
                case 'p': piece = new Pawn(color); break;
                case 'r': piece = new Rook(color); break;
                case 'n': piece = new Knight(color); break;
                case 'b': piece = new Bishop(color); break;
                case 'q': piece = new Queen(color); break;
                case 'k': piece = new King(color); break;
            }

            board[row][col++] = piece;
        }
    }

    currentTurn = (activeColor == "w") ? PieceColor::WHITE : PieceColor::BLACK;

    hasWhiteKingMoved = castling.find('K') == std::string::npos || castling.find('Q') == std::string::npos;
    hasWhiteKingsideRookMoved = castling.find('K') == std::string::npos;
    hasWhiteQueensideRookMoved = castling.find('Q') == std::string::npos;
    hasBlackKingMoved = castling.find('k') == std::string::npos || castling.find('q') == std::string::npos;
    hasBlackKingsideRookMoved = castling.find('k') == std::string::npos;
    hasBlackQueensideRookMoved = castling.find('q') == std::string::npos;

}

bool Board::hasKingMoved(PieceColor color) {
    return (color == PieceColor::WHITE) ? hasWhiteKingMoved : hasBlackKingMoved;
}

bool Board::hasRookMoved(PieceColor color, bool kingside) {
    if (color == PieceColor::WHITE) {
        return kingside ? hasWhiteKingsideRookMoved : hasWhiteQueensideRookMoved;
    } else {
        return kingside ? hasBlackKingsideRookMoved : hasBlackQueensideRookMoved;
    }
}
void Board::setKingMoved(PieceColor color) {
    if (color == PieceColor::WHITE) {
        hasWhiteKingMoved = true;
    } else {
        hasBlackKingMoved = true;
    }
}

void Board::setRookMoved(PieceColor color, bool kingside) {
    if (color == PieceColor::WHITE) {
        if (kingside) hasWhiteKingsideRookMoved = true;
        else hasWhiteQueensideRookMoved = true;
    } else {
        if (kingside) hasBlackKingsideRookMoved = true;
        else hasBlackQueensideRookMoved = true;
    }
}

bool Board::isValidCastlingMove(int fromRow, int fromCol, int toRow, int toCol, PieceColor color) {
    if (color == PieceColor::WHITE) {
        if (fromRow == 7 && fromCol == 4 && toRow == 7 && toCol == 6) {
            return !hasWhiteKingMoved && !hasWhiteKingsideRookMoved;
        } else if (fromRow == 7 && fromCol == 4 && toRow == 7 && toCol == 2) {
            return !hasWhiteKingMoved && !hasWhiteQueensideRookMoved;
        }
    } else {
        if (fromRow == 0 && fromCol == 4 && toRow == 0 && toCol == 6) {
            return !hasBlackKingMoved && !hasBlackKingsideRookMoved;
        } else if (fromRow == 0 && fromCol == 4 && toRow == 0 && toCol == 2) {
            return !hasBlackKingMoved && !hasBlackQueensideRookMoved;
        }
    }
    return false;
}

bool Board::isCheckmate(PieceColor color) const {
    return isInCheck(color) && !hasLegalMoves(color);
}

bool Board::isStalemate(PieceColor color) const {
    return !isInCheck(color) && !hasLegalMoves(color);
}

bool Board::hasLegalMoves(PieceColor color) const {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* piece = board[r][c];
            if (piece && piece->getColor() == color) {
                for (int destR = 0; destR < 8; ++destR) {
                    for (int destC = 0; destC < 8; ++destC) {
                        if (r == destR && c == destC) continue;
                        
                        if (piece->isValidMove(r, c, destR, destC, const_cast<Piece* (*)[8]>(board))) {
                            if (!isInCheckAfterMove(r, c, destR, destC, color)) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

void Board::recordPosition() {
    std::string key = generatePositionKey();
    positionHistory[key]++;
}

bool Board::isThreefoldRepetition() const {
    std::string key = generatePositionKey();
    auto it = positionHistory.find(key);
    return (it != positionHistory.end() && it->second >= 3);
}

void Board::resetPositionHistory() {
    positionHistory.clear();
    std::string key = generatePositionKey();
    positionHistory[key] = 1;
}


GameResult Board::getGameResult() const {
    if (isCheckmate(currentTurn)) return GameResult::Checkmate;
    if (isStalemate(currentTurn)) return GameResult::Stalemate;
    if (halfmoveClock >= 100) return GameResult::FiftyMoveRule;
    if (isThreefoldRepetition()) return GameResult::ThreefoldRepetition;
    return GameResult::Ongoing;
}

std::string Board::generatePositionKey() const {
    std::string key;

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = board[r][c];
            if (!p) {
                key += ".";
            } else {
                char symbol;
                switch (p->getType()) {
                    case PieceType::PAWN: symbol = 'P'; break;
                    case PieceType::ROOK: symbol = 'R'; break;
                    case PieceType::KNIGHT: symbol = 'N'; break;
                    case PieceType::BISHOP: symbol = 'B'; break;
                    case PieceType::QUEEN: symbol = 'Q'; break;
                    case PieceType::KING: symbol = 'K'; break;
                    default: symbol = '.'; break;
                }
                key += (p->getColor() == PieceColor::WHITE) ? symbol : std::tolower(symbol);
            }
        }
    }

    key += (currentTurn == PieceColor::WHITE) ? " w" : " b";

    if (!hasWhiteKingMoved && !hasWhiteKingsideRookMoved) key += " K";
    if (!hasWhiteKingMoved && !hasWhiteQueensideRookMoved) key += " Q";
    if (!hasBlackKingMoved && !hasBlackKingsideRookMoved) key += " k";
    if (!hasBlackKingMoved && !hasBlackQueensideRookMoved) key += " q";

    if (lastMove.pieceMoved &&
        lastMove.pieceMoved->getType() == PieceType::PAWN &&
        std::abs(lastMove.toRow - lastMove.fromRow) == 2) {
        
        int epRow = (lastMove.fromRow + lastMove.toRow) / 2;
        int epCol = lastMove.fromCol;
        char file = 'a' + epCol;
        char rank = '8' - epRow;
        key += " ep";
        key += file;
        key += rank;
    }


    return key;
}

