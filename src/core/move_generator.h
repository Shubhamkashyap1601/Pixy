#pragma once

#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "board.h"

class MoveGenerator {
public:
    static int countAllLegalMoves(const Board& board);
};

#endif
