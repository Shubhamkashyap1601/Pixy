#pragma once

#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "board.h"
#include<vector>

class MoveGenerator {
public:
    static int countAllLegalMoves(const Board& board);
    std::vector<Move> generateLegalMoves(const Board& board, PieceColor color);
};

#endif
