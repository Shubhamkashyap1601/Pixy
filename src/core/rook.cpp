#include "rook.h"
#include "king.h"
#include <math.h>

Rook::Rook(PieceColor color) : Piece(color) {}

Piece* Rook::clone() const {
    return new Rook(*this);
}

PieceType Rook::getType() const {
    return PieceType::ROOK;
}

bool Rook::isValidMove(int fromRow, int fromCol, int toRow, int toCol, Piece* board[8][8]) const {
    if (!hasMoved) {
        if (fromRow == toRow && (toCol == fromCol + 2 || toCol == fromCol - 2)) {
            int step = (toCol > fromCol) ? 1 : -1;
            for (int col = fromCol + step; col != toCol; col += step) {
                if (board[fromRow][col]) return false; // Blocked by another piece
            }

            King* king = dynamic_cast<King*>(board[fromRow][fromCol]);
            if (king && !king->hasMoved) {
                int rookCol = (toCol > fromCol) ? 7 : 0; 
                if (board[fromRow][rookCol] && board[fromRow][rookCol]->getType() == PieceType::ROOK) {
                    Rook* castlingRook = dynamic_cast<Rook*>(board[fromRow][rookCol]);
                    if (castlingRook && !castlingRook->hasMoved) {
                        return true;
                    }
                }
            }
        }
    }

    if (fromRow == toRow) {
        int step = (toCol > fromCol ? 1 : -1);
        for (int col = fromCol + step; col != toCol; col += step) {
            if (board[fromRow][col]) return false; // Blocked by another piece
        }
        if (!board[toRow][toCol] || board[toRow][toCol]->getColor() != getColor()) {
            return true; 
        }
    }

    if (fromCol == toCol) {
        int step = (toRow > fromRow ? 1 : -1);
        for (int row = fromRow + step; row != toRow; row += step) {
            if (board[row][fromCol]) return false; 
        }
        if (!board[toRow][toCol] || board[toRow][toCol]->getColor() != getColor()) {
            return true; 
        }
    }

    return false;
}
