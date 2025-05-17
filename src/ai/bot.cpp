#include "bot.h"
#include "../core/move_generator.h"
#include "evaluate.h"
#include <limits>
#include <algorithm>
#include <stdexcept>


Move Bot::findBestMove(Board& board, int depth) {
    int bestValue = std::numeric_limits<int>::min();
    
    MoveGenerator moveGenerator;
    std::vector<Move> moves = moveGenerator.generateLegalMoves(board, board.getCurrentTurn());
    
    if (moves.empty()) {
        // No legal moves found
        throw std::runtime_error("No legal moves available");
    }
    
    Move bestMove = moves[0]; // Initialize with first move
    for (const auto& move : moves) {
        Board temp = board;
        temp.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);

        int value = minimax(temp, depth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
        if (value > bestValue) {
            bestValue = value;
            bestMove = move;
        }
    }
    return bestMove;
}
int Bot::minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0) return evaluate(board);

    MoveGenerator moveGenerator;
    std::vector<Move> moves = moveGenerator.generateLegalMoves(board, board.getCurrentTurn());

    if (moves.empty()) return evaluate(board);
 // Terminal node (checkmate/stalemate)
    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        for (const auto& move : moves) {
            Board temp = board;
            temp.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
            int eval = minimax(temp, depth - 1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (const auto& move : moves) {
            Board temp = board;
            temp.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
            int eval = minimax(temp, depth - 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

int Bot::evaluateBoard(const Board& board) {
    int score = 0;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = board.getPiece(row, col);
            if (!piece) continue;

            int value = 0;
            switch (piece->getType()) {
                case PieceType::PAWN: value = 100; break;
                case PieceType::KNIGHT: value = 320; break;
                case PieceType::BISHOP: value = 330; break;
                case PieceType::ROOK: value = 500; break;
                case PieceType::QUEEN: value = 900; break;
                case PieceType::KING: value = 20000; break;
            }

            if (piece->getColor() == PieceColor::WHITE)
                score += value;
            else
                score -= value;
        }
    }
    return score;
}
