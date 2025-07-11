#pragma once

#include "../core/board.h"
#include "../core/move.h"
#include <vector>

class Bot {
public:
    Bot() : nodesSearched(0), ttHits(0) {
        // Initialize transposition table
        transpositionTable.resize(TT_SIZE);
        for (int i = 0; i < TT_SIZE; i++) {
            transpositionTable[i].isValid = false;
        }
    }
    
    Move findBestMove(Board& board, int depth);

private:
    int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer);
    int quiescence(Board& board, int alpha, int beta);
    std::vector<Move> generateNoisyMoves(Board& board, bool whiteToMove);
    int evaluateBoard(const Board& board);
    
    // Enhanced move ordering
    std::vector<Move> orderMoves(Board& board, const std::vector<Move>& moves);
    int getMoveOrderingScore(Board& board, const Move& move);
    
    // Opening principles
    bool isGoodOpeningMove(Board& board, const Move& move);
    int getOpeningBonus(Board& board, const Move& move);
    
    // Enhanced evaluation helpers
    int evaluateKingSafety(const Board& board, PieceColor color);
    int evaluatePawnStructure(const Board& board, PieceColor color);
    int evaluatePieceActivity(const Board& board, PieceColor color);
    
    // Game phase detection
    bool isOpeningPhase(const Board& board);
    bool isEndgamePhase(const Board& board);
    
    // Performance optimizations
    struct TranspositionEntry {
        int score;
        int depth;
        Move bestMove;
        bool isValid;
    };
    
    // Simple hash table for transposition table
    static const int TT_SIZE = 65536; // 64K entries (much smaller)
    std::vector<TranspositionEntry> transpositionTable;
    
    uint64_t calculateHash(const Board& board);
    bool probeTT(uint64_t hash, int depth, int alpha, int beta, int& score, Move& bestMove);
    void storeTT(uint64_t hash, int depth, int score, const Move& bestMove);
    
    // Iterative deepening
    Move iterativeDeepening(Board& board, int maxDepth);
    
    // Performance tracking
    int nodesSearched;
    int ttHits;
};
