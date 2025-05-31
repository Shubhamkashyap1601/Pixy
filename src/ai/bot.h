#pragma once

#include "../core/board.h"
#include "../core/move.h"
#include <vector>

class Bot {
public:
    Move findBestMove(Board& board, int depth);

private:
    int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer);
        int quiescence(Board& board, int alpha, int beta);
    std::vector<Move> generateNoisyMoves(Board& board, bool whiteToMove);
    int evaluateBoard(const Board& board);
};
