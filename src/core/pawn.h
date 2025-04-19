#pragma once
#include "piece.h"
#include <math.h>
#include "move.h"
#include <iostream>

class Pawn : public Piece {
public:
    Pawn(PieceColor color);
    Piece* clone() const override;
    PieceType getType() const override;
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, Piece* board[8][8]) const override;
    bool canEnPassant(int fromRow, int fromCol, int toRow, int toCol, Piece* board[8][8], const Move& lastMove) const;
};
