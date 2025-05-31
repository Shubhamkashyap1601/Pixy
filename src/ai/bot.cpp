#include "bot.h"
#include "../core/move_generator.h"
#include "evaluate.h"
#include <limits>
#include <algorithm>
#include <stdexcept>
#include <iostream>

Move Bot::findBestMove(Board &board, int depth) {
    int bestValue = std::numeric_limits<int>::min();
    MoveGenerator moveGenerator;
    std::vector<Move> moves = moveGenerator.generateLegalMoves(board, board.getCurrentTurn());

    if (moves.empty())
        throw std::runtime_error("No legal moves available");

    std::vector<std::pair<Move, int>> scoredMoves;
    for (const auto &move : moves) {
        Board temp = board;
        temp.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
        int score = evaluate(temp);
        scoredMoves.emplace_back(move, score);
    }

    std::sort(scoredMoves.begin(), scoredMoves.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });

    Move bestMove = scoredMoves[0].first;

    for (const auto &[move, _] : scoredMoves) {
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

int Bot::minimax(Board &board, int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0)
        return quiescence(board, alpha, beta);

    MoveGenerator moveGenerator;
    std::vector<Move> moves = moveGenerator.generateLegalMoves(board, board.getCurrentTurn());

    if (moves.empty())
        return evaluate(board);

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        // Move ordering: prioritize captures, then sort by static evaluation (descending)
        std::vector<std::tuple<Move, int, bool>> scoredMoves;
        for (const auto &move : moves) {
            Board temp = board;
            Piece* captured = board.getPiece(move.toRow, move.toCol);
            temp.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
            int score = evaluate(temp);
            bool isCapture = (captured != nullptr);
            scoredMoves.emplace_back(move, score, isCapture);
        }
        std::sort(scoredMoves.begin(), scoredMoves.end(), [](const auto &a, const auto &b) {
            if (std::get<2>(a) != std::get<2>(b))
                return std::get<2>(a) > std::get<2>(b); // Captures first
            return std::get<1>(a) > std::get<1>(b); // Then by evaluation
        });
        for (const auto &[move, _, __] : scoredMoves) {
            Board temp = board;
            temp.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
            int eval = minimax(temp, depth - 1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break;
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        // Move ordering: prioritize captures, then sort by static evaluation (ascending)
        std::vector<std::tuple<Move, int, bool>> scoredMoves;
        for (const auto &move : moves) {
            Board temp = board;
            Piece* captured = board.getPiece(move.toRow, move.toCol);
            temp.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
            int score = evaluate(temp);
            bool isCapture = (captured != nullptr);
            scoredMoves.emplace_back(move, score, isCapture);
        }
        std::sort(scoredMoves.begin(), scoredMoves.end(), [](const auto &a, const auto &b) {
            if (std::get<2>(a) != std::get<2>(b))
                return std::get<2>(a) > std::get<2>(b); // Captures first
            return std::get<1>(a) < std::get<1>(b); // Then by evaluation
        });
        for (const auto &[move, _, __] : scoredMoves) {
            Board temp = board;
            temp.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
            int eval = minimax(temp, depth - 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break;
        }
        return minEval;
    }
}


int Bot::quiescence(Board &board, int alpha, int beta) {
    int standPat = evaluate(board);

    if (standPat >= beta)
        return beta;
    if (alpha < standPat)
        alpha = standPat;

    MoveGenerator moveGenerator;
    auto moves = moveGenerator.generateLegalMoves(board, board.getCurrentTurn());

    for (const auto &move : moves) {
        Piece *captured = board.getPiece(move.toRow, move.toCol);
        if (!captured)
            continue;

        Board temp = board;
        temp.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
        int score = -quiescence(temp, -beta, -alpha);

        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }
    return alpha;
}
