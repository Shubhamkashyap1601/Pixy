#include "knight.h"
#include<math.h>
Knight::Knight(PieceColor color) : Piece(color) {}

Piece *Knight::clone() const
{
    return new Knight(*this);
}

PieceType Knight::getType() const {
    return PieceType::KNIGHT;
}

bool Knight::isValidMove(int fromRow, int fromCol, int toRow, int toCol, Piece* board[8][8]) const {
    int dRow = abs(toRow - fromRow);
    int dCol = abs(toCol - fromCol);
    if ((dRow == 2 && dCol == 1) || (dRow == 1 && dCol == 2)) {
        return !board[toRow][toCol] || board[toRow][toCol]->getColor() != getColor();
    }
    return false;
}
