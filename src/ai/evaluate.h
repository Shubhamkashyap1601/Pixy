#pragma once
#include "../core/board.h"

int evaluate(const Board& board);
int evaluateIsolatedPawns(const Board& board, PieceColor color);
