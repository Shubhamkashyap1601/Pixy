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

    Move(int fr, int fc, int tr, int tc, Piece* pm)
        : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc), pieceMoved(pm) {}
};

#endif
