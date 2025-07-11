#ifndef MOVE_H
#define MOVE_H

#include "piece.h"

class Move {
public:
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;
    Piece* pieceMoved;

    // Default constructor
    Move() : fromRow(0), fromCol(0), toRow(0), toCol(0), pieceMoved(nullptr) {}
    
    Move(int fr, int fc, int tr, int tc, Piece* pm)
        : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc), pieceMoved(pm) {}
};

#endif
