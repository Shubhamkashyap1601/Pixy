#include "queen.h"
#include "rook.h"
#include "bishop.h"

Queen::Queen(PieceColor color) : Piece(color) {}

Piece *Queen::clone() const
{
    return new Queen(*this);
}

PieceType Queen::getType() const {
    return PieceType::QUEEN;
}

bool Queen::isValidMove(int fromRow, int fromCol, int toRow, int toCol, Piece* board[8][8]) const {
    Rook rookPiece(getColor());
    Bishop bishopPiece(getColor());

    return rookPiece.isValidMove(fromRow, fromCol, toRow, toCol, board) || 
           bishopPiece.isValidMove(fromRow, fromCol, toRow, toCol, board);
}
