#include "bot.h"
#include "../core/move_generator.h"
#include "evaluate.h"
#include <limits>
#include <algorithm>
#include <stdexcept>
#include <iostream>

Move Bot::findBestMove(Board &board, int depth) {
    nodesSearched = 0;
    ttHits = 0;
    
    // Clear or initialize transposition table occasionally
    static int callCount = 0;
    if (++callCount % 10 == 0) {
        for (int i = 0; i < TT_SIZE; i++) {
            transpositionTable[i].isValid = false;
        }
    }
    
    Move bestMove = iterativeDeepening(board, depth);
    
    std::cout << "Nodes searched: " << nodesSearched << ", TT hits: " << ttHits << std::endl;
    return bestMove;
}

Move Bot::iterativeDeepening(Board& board, int maxDepth) {
    MoveGenerator moveGenerator;
    std::vector<Move> moves = moveGenerator.generateLegalMoves(board, board.getCurrentTurn());
    
    if (moves.empty())
        throw std::runtime_error("No legal moves available");
    
    Move bestMove = moves[0];
    int bestScore = std::numeric_limits<int>::min();
    
    // Iterative deepening - start shallow and go deeper
    for (int depth = 1; depth <= maxDepth; depth++) {
        int currentBestScore = std::numeric_limits<int>::min();
        Move currentBestMove = bestMove;
        
        // Order moves based on previous iteration's best move
        moves = orderMoves(board, moves);
        
        for (const auto &move : moves) {
            Board temp = board;
            temp.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
            int score = minimax(temp, depth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
            
            if (score > currentBestScore) {
                currentBestScore = score;
                currentBestMove = move;
            }
        }
        
        bestMove = currentBestMove;
        bestScore = currentBestScore;
        
        std::cout << "Depth " << depth << " completed. Best score: " << bestScore << std::endl;
    }
    
    return bestMove;
}

int Bot::minimax(Board &board, int depth, int alpha, int beta, bool maximizingPlayer) {
    nodesSearched++;
    
    // Check transposition table
    uint64_t hash = calculateHash(board);
    Move ttMove;
    int ttScore;
    if (probeTT(hash, depth, alpha, beta, ttScore, ttMove)) {
        ttHits++;
        return ttScore;
    }
    
    if (depth == 0)
        return quiescence(board, alpha, beta);

    MoveGenerator moveGenerator;
    std::vector<Move> moves = moveGenerator.generateLegalMoves(board, board.getCurrentTurn());

    if (moves.empty())
        return evaluate(board);

    Move bestMove;
    int bestScore;

    if (maximizingPlayer) {
        bestScore = std::numeric_limits<int>::min();
        moves = orderMoves(board, moves);
        
        for (size_t i = 0; i < moves.size(); i++) {
            const auto &move = moves[i];
            Board temp = board;
            temp.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
            
            int searchDepth = depth - 1;
            // Late Move Reduction: reduce depth for later moves if they're not captures
            if (i > 3 && depth > 2 && !board.getPiece(move.toRow, move.toCol)) {
                searchDepth = depth - 2;
            }
            
            int eval = minimax(temp, searchDepth, alpha, beta, false);
            
            if (eval > bestScore) {
                bestScore = eval;
                bestMove = move;
            }
            
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break; // Alpha-beta cutoff
        }
    } else {
        bestScore = std::numeric_limits<int>::max();
        moves = orderMoves(board, moves);
        
        for (size_t i = 0; i < moves.size(); i++) {
            const auto &move = moves[i];
            Board temp = board;
            temp.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol);
            
            int searchDepth = depth - 1;
            // Late Move Reduction
            if (i > 3 && depth > 2 && !board.getPiece(move.toRow, move.toCol)) {
                searchDepth = depth - 2;
            }
            
            int eval = minimax(temp, searchDepth, alpha, beta, true);
            
            if (eval < bestScore) {
                bestScore = eval;
                bestMove = move;
            }
            
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break; // Alpha-beta cutoff
        }
    }
    
    // Store in transposition table
    storeTT(hash, depth, bestScore, bestMove);
    
    return bestScore;
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

std::vector<Move> Bot::orderMoves(Board& board, const std::vector<Move>& moves) {
    std::vector<std::pair<Move, int>> scoredMoves;
    
    for (const auto& move : moves) {
        int score = getMoveOrderingScore(board, move);
        scoredMoves.emplace_back(move, score);
    }
    
    std::sort(scoredMoves.begin(), scoredMoves.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<Move> orderedMoves;
    for (const auto& [move, _] : scoredMoves) {
        orderedMoves.push_back(move);
    }
    
    return orderedMoves;
}

int Bot::getMoveOrderingScore(Board& board, const Move& move) {
    int score = 0;
    
    // Prioritize captures
    Piece* captured = board.getPiece(move.toRow, move.toCol);
    if (captured) {
        Piece* moving = board.getPiece(move.fromRow, move.fromCol);
        if (moving) {
            // MVV-LVA (Most Valuable Victim - Least Valuable Attacker)
            score += 1000 + (int)captured->getType() * 100 - (int)moving->getType() * 10;
        }
    }
    
    // Opening principles bonus (includes penalties for bad moves)
    if (isOpeningPhase(board)) {
        score += getOpeningBonus(board, move);
    }
    
    // Center control bonus
    if ((move.toRow == 3 || move.toRow == 4) && (move.toCol == 3 || move.toCol == 4)) {
        score += 50;
    }
    
    // Penalize moving the same piece twice in opening
    if (isOpeningPhase(board)) {
        Piece* piece = board.getPiece(move.fromRow, move.fromCol);
        if (piece && piece->getType() != PieceType::PAWN) {
            // Check if this piece is already developed
            bool isBackRank = (piece->getColor() == PieceColor::WHITE && move.fromRow == 7) ||
                             (piece->getColor() == PieceColor::BLACK && move.fromRow == 0);
            if (!isBackRank) {
                score -= 30; // Penalty for moving already developed piece
            }
        }
    }
    
    return score;
}

bool Bot::isGoodOpeningMove(Board& board, const Move& move) {
    Piece* piece = board.getPiece(move.fromRow, move.fromCol);
    if (!piece) return false;
    
    // Good opening moves
    if (piece->getType() == PieceType::PAWN) {
        // Center pawn moves
        if (move.fromCol == 3 || move.fromCol == 4) { // d2-d4, e2-e4
            if (abs(move.toRow - move.fromRow) == 2) return true;
        }
    }
    
    if (piece->getType() == PieceType::KNIGHT) {
        // Knight development to good squares
        if ((move.toRow == 5 && (move.toCol == 2 || move.toCol == 5)) || // Nf3, Nc3
            (move.toRow == 2 && (move.toCol == 2 || move.toCol == 5))) { // Nf6, Nc6
            return true;
        }
    }
    
    if (piece->getType() == PieceType::BISHOP) {
        // Bishop development
        if (move.toRow == 5 || move.toRow == 2) { // Third or sixth rank
            return true;
        }
    }
    
    return false;
}

int Bot::getOpeningBonus(Board& board, const Move& move) {
    Piece* piece = board.getPiece(move.fromRow, move.fromCol);
    if (!piece) return 0;
    
    int bonus = 0;
    
    // HEAVY penalties for terrible opening moves
    if (piece->getType() == PieceType::PAWN) {
        // Massively penalize f7-f6, f2-f3, h7-h6, h2-h3, a7-a6, a2-a3 type moves
        if (move.fromCol == 5 && move.toCol == 5) { // f-file pawn moves
            bonus -= 500; // Massive penalty for f6/f3
        }
        if (move.fromCol == 7 && move.toCol == 7) { // h-file pawn moves  
            bonus -= 300; // Heavy penalty for h6/h3
        }
        if (move.fromCol == 0 && move.toCol == 0) { // a-file pawn moves
            bonus -= 300; // Heavy penalty for a6/a3
        }
        if (move.fromCol == 2 && move.toCol == 2) { // c-file pawn moves (c6 is okay, but c5 is better)
            if (abs(move.toRow - move.fromRow) == 1) {
                bonus -= 150; // Penalty for c6/c3 one-square moves
            }
        }
        
        // Good center pawn moves get big bonus
        if ((move.fromCol == 3 || move.fromCol == 4) && abs(move.toRow - move.fromRow) == 2) {
            bonus += 200; // Big bonus for e4, d4, e5, d5
        }
    }
    
    if (piece->getType() == PieceType::KNIGHT) {
        // Heavily penalize knight moves to rim
        if (move.toRow == 0 || move.toRow == 7 || move.toCol == 0 || move.toCol == 7) {
            bonus -= 400; // Massive penalty for knights on rim
        }
        
        // Good knight development squares
        if ((move.toRow == 5 && (move.toCol == 2 || move.toCol == 5)) || // Nf3, Nc3
            (move.toRow == 2 && (move.toCol == 2 || move.toCol == 5))) { // Nf6, Nc6
            bonus += 150;
        }
    }
    
    if (piece->getType() == PieceType::BISHOP) {
        // Good bishop development
        bonus += 100;
    }
    
    if (piece->getType() == PieceType::KING) {
        // MASSIVE penalty for early king moves
        bonus -= 800;
    }
    
    if (piece->getType() == PieceType::QUEEN) {
        // Don't develop queen early
        bonus -= 200;
    }
    
    return bonus;
}

bool Bot::isOpeningPhase(const Board& board) {
    int moveCount = 0;
    int developedPieces = 0;
    
    // Count developed pieces (not on back rank)
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece* piece = board.getPiece(row, col);
            if (piece && (piece->getType() == PieceType::KNIGHT || piece->getType() == PieceType::BISHOP)) {
                bool onBackRank = (piece->getColor() == PieceColor::WHITE && row == 7) ||
                                 (piece->getColor() == PieceColor::BLACK && row == 0);
                if (!onBackRank) {
                    developedPieces++;
                }
            }
        }
    }
    
    // Opening if less than 6 minor pieces developed
    return developedPieces < 6;
}

bool Bot::isEndgamePhase(const Board& board) {
    int materialCount = 0;
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece* piece = board.getPiece(row, col);
            if (piece && piece->getType() != PieceType::KING && piece->getType() != PieceType::PAWN) {
                materialCount++;
            }
        }
    }
    
    // Endgame if less than 6 pieces total (excluding kings and pawns)
    return materialCount <= 6;
}

uint64_t Bot::calculateHash(const Board& board) {
    // Simple hash function - in a real engine you'd use Zobrist hashing
    uint64_t hash = 0;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece* piece = board.getPiece(row, col);
            if (piece) {
                hash ^= ((uint64_t)piece->getType() << (row * 8 + col)) + 
                       ((uint64_t)piece->getColor() << (row * 8 + col + 32));
            }
        }
    }
    hash ^= (uint64_t)board.getCurrentTurn() << 60;
    return hash;
}

bool Bot::probeTT(uint64_t hash, int depth, int alpha, int beta, int& score, Move& bestMove) {
    int index = hash % TT_SIZE;
    TranspositionEntry& entry = transpositionTable[index];
    
    if (!entry.isValid) return false;
    
    if (entry.depth >= depth) {
        score = entry.score;
        bestMove = entry.bestMove;
        
        // Check if we can use this score
        if (score <= alpha || score >= beta) {
            return true;
        }
    }
    
    return false;
}

void Bot::storeTT(uint64_t hash, int depth, int score, const Move& bestMove) {
    int index = hash % TT_SIZE;
    TranspositionEntry& entry = transpositionTable[index];
    
    // Always replace (simple replacement scheme)
    entry.score = score;
    entry.depth = depth;
    entry.bestMove = bestMove;
    entry.isValid = true;
}
