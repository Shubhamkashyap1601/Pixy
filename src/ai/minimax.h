#pragma once
#include "../core/board.h"
#include "../core/move.h"

Move findBestMove(Board& board, int depth, PieceColor botColor);
int minimax(Board& board, int depth, bool isMaximizing, PieceColor botColor);