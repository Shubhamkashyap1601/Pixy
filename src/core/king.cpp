#include "king.h"
#include "rook.h"
#include <cmath>

King::King(PieceColor color) : Piece(color) {}

Piece* King::clone() const {
    return new King(*this);
}

PieceType King::getType() const {
    return PieceType::KING;
}

bool King::isValidMove(int fromRow, int fromCol, int toRow, int toCol, Piece* board[8][8]) const {
    int dRow = abs(toRow - fromRow);
    int dCol = abs(toCol - fromCol);

    if ((dRow == 1 && dCol <= 1) || (dRow <= 1 && dCol == 1)) {
        return !board[toRow][toCol] || board[toRow][toCol]->getColor() != getColor();
    }

    if (!hasMoved && !board[toRow][toCol] && toRow == fromRow && (toCol == fromCol + 2 || toCol == fromCol - 2)) {
        Rook* rook = dynamic_cast<Rook*>(board[fromRow][toCol > fromCol ? 7 : 0]);
        if (rook && !rook->hasMoved) {
            return true;
        }
    }

    return false;
}

