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

void Board::printMessage(const std::string& msg) {
    if (!isBotPlaying) {
        std::cout << msg << std::endl;
    }
}


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
    if (fromRow < 0 || fromRow >= 8 || fromCol < 0 || fromCol >= 8 ||
        toRow < 0 || toRow >= 8 || toCol < 0 || toCol >= 8) {
        printMessage("Move coordinates out of bounds!");
        return false;
}

    Piece* piece = board[fromRow][fromCol];
    if (!piece) {
        printMessage("Invalid move (no piece at source).");
        return false;
    }

    if (piece->getColor() != currentTurn) {
        printMessage(std::string("It's not ") + (currentTurn == PieceColor::WHITE ? "White" : "Black") + "'s turn!");
        return false;
    }

    bool isEnPassant = false;
    int enPassantCapturedRow = -1, enPassantCapturedCol = -1;

    if (piece->getType() == PieceType::PAWN) {
        Pawn* pawn = static_cast<Pawn*>(piece);

        // En passant detection:
        if (lastMove.pieceMoved && lastMove.pieceMoved->getType() == PieceType::PAWN) {
            int lastFromRow = lastMove.fromRow;
            int lastToRow = lastMove.toRow;
            int lastToCol = lastMove.toCol;

            // Check if last move was a two-step pawn move
            if (std::abs(lastFromRow - lastToRow) == 2) {
                // If current pawn tries to move diagonally behind that pawn
                int direction = (pawn->getColor() == PieceColor::WHITE) ? -1 : 1;
                if (toRow == fromRow + direction &&
                    std::abs(toCol - fromCol) == 1 &&
                    toRow == lastToRow &&
                    toCol == lastToCol) {

                    isEnPassant = true;
                    enPassantCapturedRow = lastToRow;
                    enPassantCapturedCol = lastToCol;
                }
            }
        }
    }

    // Castling logic with all fixes included
    if (piece->getType() == PieceType::KING && std::abs(toCol - fromCol) == 2 && fromRow == toRow) {
        bool isKingside = toCol > fromCol;
        int row = fromRow;
        int rookCol = isKingside ? 7 : 0;
        int rookDestCol = isKingside ? 5 : 3;
        int step = isKingside ? 1 : -1;

        // Check rook presence and color
        Piece* rook = board[row][rookCol];
        if (!rook || rook->getType() != PieceType::ROOK || rook->getColor() != piece->getColor()) {
            printMessage("Castling invalid: Rook missing or wrong color.");
            return false;
        }

        // Check if king or rook has moved
        if (piece->getColor() == PieceColor::WHITE) {
            if (hasWhiteKingMoved) {
                printMessage("Castling invalid: White king has already moved.");
                return false;
            }
            if (isKingside && hasWhiteKingsideRookMoved) {
                printMessage("Castling invalid: White kingside rook has already moved.");
                return false;
            }
            if (!isKingside && hasWhiteQueensideRookMoved) {
                printMessage("Castling invalid: White queenside rook has already moved.");
                return false;
            }
        } else {
            if (hasBlackKingMoved) {
                printMessage("Castling invalid: Black king has already moved.");
                return false;
            }
            if (isKingside && hasBlackKingsideRookMoved) {
                printMessage("Castling invalid: Black kingside rook has already moved.");
                return false;
            }
            if (!isKingside && hasBlackQueensideRookMoved) {
                printMessage("Castling invalid: Black queenside rook has already moved.");
                return false;
            }
        }

        // Check if path between king and rook is clear
        for (int c = fromCol + step; c != rookCol; c += step) {
            if (board[row][c]) {
                printMessage("Castling blocked: path not clear.");
                return false;
            }
        }

        // Check if king is currently in check
        if (isInCheck(piece->getColor())) {
            printMessage("Castling invalid: King is currently in check.");
            return false;
        }

        // Check if king passes through check squares
        Piece* origKing = board[row][fromCol];
        for (int i = 1; i <= 2; ++i) {
            int c = fromCol + step * i;
            Piece* origDest = board[row][c];

            board[row][fromCol] = nullptr;
            board[row][c] = origKing;

            if (isInCheck(piece->getColor())) {
                board[row][c] = origDest;
                board[row][fromCol] = origKing;
                printMessage("Castling invalid: King would pass through check.");
                return false;
            }

            board[row][c] = origDest;
            board[row][fromCol] = origKing;
        }

        // Perform castling move
        board[row][toCol] = piece;
        board[row][fromCol] = nullptr;

        board[row][rookDestCol] = rook;
        board[row][rookCol] = nullptr;

        // Update castling flags
        if (piece->getColor() == PieceColor::WHITE) {
            hasWhiteKingMoved = true;
            if (isKingside) hasWhiteKingsideRookMoved = true;
            else hasWhiteQueensideRookMoved = true;
        } else {
            hasBlackKingMoved = true;
            if (isKingside) hasBlackKingsideRookMoved = true;
            else hasBlackQueensideRookMoved = true;
        }

        lastMove = Move(fromRow, fromCol, toRow, toCol, piece);

        printMessage("Castling executed!");
        currentTurn = (currentTurn == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;

        return true;
    }

    // Normal move validation for pieces (except en passant)
    if (!isEnPassant && !piece->isValidMove(fromRow, fromCol, toRow, toCol, board)) {
        printMessage("Invalid move for this piece!");
        return false;
    }

    Piece* captured = board[toRow][toCol];

    if (isEnPassant) {
        // Remove the captured pawn properly
        Piece* capturedPawn = board[enPassantCapturedRow][enPassantCapturedCol];
        if (capturedPawn && capturedPawn->getType() == PieceType::PAWN) {
            delete capturedPawn;
            board[enPassantCapturedRow][enPassantCapturedCol] = nullptr;
            printMessage("En passant captured!");
        } else {
            printMessage("En passant capture failed: no pawn to capture.");
            return false;
        }
    }

    // Move piece on board
    board[toRow][toCol] = piece;
    board[fromRow][fromCol] = nullptr;

    // Check if move results in own king in check
    if (isInCheck(piece->getColor())) {
        // Undo move
        board[fromRow][fromCol] = piece;
        board[toRow][toCol] = captured;

        Piece* capturedPawn = nullptr;

        if (isEnPassant) {
            // capturedPawn = board[enPassantCapturedRow][enPassantCapturedCol];
            // if (capturedPawn && capturedPawn->getType() == PieceType::PAWN) {
            //     delete capturedPawn;
                board[enPassantCapturedRow][enPassantCapturedCol] = nullptr;
            // } else {
            //     printMessage("En passant capture failed: no pawn to capture.");
            //     return false;
            // }
        }

        printMessage("Move puts king in check! Invalid.");
        return false;
    }

    // Delete captured piece if any and not en passant (already deleted above)
    if (captured && !isEnPassant) delete captured;

    // Handle pawn promotion
    if (piece->getType() == PieceType::PAWN) {
        if ((piece->getColor() == PieceColor::WHITE && toRow == 0) ||
            (piece->getColor() == PieceColor::BLACK && toRow == 7)) {

            PieceColor color = piece->getColor();
            delete piece;

            if (isBotPlaying) {
                piece = new Queen(color);
            } else {
                char choice;
                std::cout << "Promote pawn to (Q, R, B, N): ";
                std::cin >> choice;
                choice = std::toupper(choice);

                switch (choice) {
                    case 'Q': piece = new Queen(color); break;
                    case 'R': piece = new Rook(color); break;
                    case 'B': piece = new Bishop(color); break;
                    case 'N': piece = new Knight(color); break;
                    default:
                        printMessage("Invalid choice. Defaulting to Queen.");
                        piece = new Queen(color);
                        break;
                }
            }

            board[toRow][toCol] = piece;
        }
    }

    lastMove = Move(fromRow, fromCol, toRow, toCol, piece);
    recordPosition();

    // Update king moved flags if king moved
    if (piece->getType() == PieceType::KING) {
        if (piece->getColor() == PieceColor::WHITE) hasWhiteKingMoved = true;
        else hasBlackKingMoved = true;
    }

    // Update rook moved flags if rook moved
    if (piece->getType() == PieceType::ROOK) {
        if (piece->getColor() == PieceColor::WHITE) {
            if (fromRow == 7 && fromCol == 0) hasWhiteQueensideRookMoved = true;
            else if (fromRow == 7 && fromCol == 7) hasWhiteKingsideRookMoved = true;
        } else {
            if (fromRow == 0 && fromCol == 0) hasBlackQueensideRookMoved = true;
            else if (fromRow == 0 && fromCol == 7) hasBlackKingsideRookMoved = true;
        }
    }

    currentTurn = (currentTurn == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;

    return true;
}


Move Board::getLastMove() const {
    return lastMove;
}

Piece* Board::getPiece(int row, int col) const {
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

Board::Board(const Board& other) : lastMove(other.lastMove), currentTurn(other.currentTurn) {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (other.board[r][c])
                board[r][c] = other.board[r][c]->clone();
            else
                board[r][c] = nullptr;
        }
    }
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

