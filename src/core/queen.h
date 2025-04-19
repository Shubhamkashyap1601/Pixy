#pragma once
#include "piece.h"

class Queen : public Piece {
public:
    Queen(PieceColor color);
    Piece* clone() const override;
    PieceType getType() const override;
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, Piece* board[8][8]) const override;
};
