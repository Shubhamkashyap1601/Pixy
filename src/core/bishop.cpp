#include "bishop.h"
#include<math.h>
Bishop::Bishop(PieceColor color) : Piece(color) {}

Piece *Bishop::clone() const
{
    return new Bishop(*this);
}

PieceType Bishop::getType() const {
    return PieceType::BISHOP;
}

bool Bishop::isValidMove(int fromRow, int fromCol, int toRow, int toCol, Piece* board[8][8]) const {
    int dRow = abs(toRow - fromRow);
    int dCol = abs(toCol - fromCol);

    if (dRow == dCol) {
        int rowDirection = (toRow > fromRow) ? 1 : -1;
        int colDirection = (toCol > fromCol) ? 1 : -1;

        int row = fromRow + rowDirection;
        int col = fromCol + colDirection;

        while (row != toRow && col != toCol) {
            if (board[row][col]) return false;
            row += rowDirection;
            col += colDirection;
        }

        return !board[toRow][toCol] || board[toRow][toCol]->getColor() != getColor();
    }

    return false;
}
