#include "evaluate.h"

namespace {


    const int pawnTable[8][8] = {
        { 0,  0,  0,  0,  0,  0,  0,  0},
        {50, 50, 50, 50, 50, 50, 50, 50},
        {10, 10, 20, 30, 30, 20, 10, 10},
        { 5,  5, 10, 25, 25, 10,  5,  5},
        { 0,  0,  0, 20, 20,  0,  0,  0},
        { 5, -5,-10,  0,  0,-10, -5,  5},
        { 5, 10, 10,-20,-20, 10, 10,  5},
        { 0,  0,  0,  0,  0,  0,  0,  0}
    };

    const int knightTable[8][8] = {
        {-50,-40,-30,-30,-30,-30,-40,-50},
        {-40,-20,  0,  5,  5,  0,-20,-40},
        {-30,  5, 10, 15, 15, 10,  5,-30},
        {-30,  0, 15, 20, 20, 15,  0,-30},
        {-30,  5, 15, 20, 20, 15,  5,-30},
        {-30,  0, 10, 15, 15, 10,  0,-30},
        {-40,-20,  0,  0,  0,  0,-20,-40},
        {-50,-40,-30,-30,-30,-30,-40,-50}
    };

    const int bishopTable[8][8] = {
        {-20,-10,-10,-10,-10,-10,-10,-20},
        {-10,  5,  0,  0,  0,  0,  5,-10},
        {-10, 10, 10, 10, 10, 10, 10,-10},
        {-10,  0, 10, 10, 10, 10,  0,-10},
        {-10,  5,  5, 10, 10,  5,  5,-10},
        {-10,  0,  5, 10, 10,  5,  0,-10},
        {-10,  0,  0,  0,  0,  0,  0,-10},
        {-20,-10,-10,-10,-10,-10,-10,-20}
    };

    const int rookTable[8][8] = {
        {  0,  0,  0,  0,  0,  0,  0,  0},
        {  5, 10, 10, 10, 10, 10, 10,  5},
        { -5,  0,  0,  0,  0,  0,  0, -5},
        { -5,  0,  0,  0,  0,  0,  0, -5},
        { -5,  0,  0,  0,  0,  0,  0, -5},
        { -5,  0,  0,  0,  0,  0,  0, -5},
        { -5,  0,  0,  0,  0,  0,  0, -5},
        {  0,  0,  0,  5,  5,  0,  0,  0}
    };

    const int queenTable[8][8] = {
        {-20,-10,-10, -5, -5,-10,-10,-20},
        {-10,  0,  0,  0,  0,  0,  0,-10},
        {-10,  0,  5,  5,  5,  5,  0,-10},
        { -5,  0,  5,  5,  5,  5,  0, -5},
        {  0,  0,  5,  5,  5,  5,  0, -5},
        {-10,  5,  5,  5,  5,  5,  0,-10},
        {-10,  0,  5,  0,  0,  0,  0,-10},
        {-20,-10,-10, -5, -5,-10,-10,-20}
    };

    const int kingTable[8][8] = {
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-20,-30,-30,-40,-40,-30,-30,-20},
        {-10,-20,-20,-20,-20,-20,-20,-10},
        { 20, 20,  0,  0,  0,  0, 20, 20},
        { 20, 30, 10,  0,  0, 10, 30, 20}
    };

    int pieceValue(PieceType type) {
        switch (type) {
            case PieceType::PAWN: return 100;
            case PieceType::KNIGHT: return 320;
            case PieceType::BISHOP: return 330;
            case PieceType::ROOK: return 500;
            case PieceType::QUEEN: return 900;
            case PieceType::KING: return 20000;
            default: return 0;
        }
    }

    int getPieceSquareValue(PieceType type, int row, int col, PieceColor color) {
        // Mirror for black pieces because tables are for white
        if (color == PieceColor::BLACK) row = 7 - row;

        switch (type) {
            case PieceType::PAWN: return pawnTable[row][col];
            case PieceType::KNIGHT: return knightTable[row][col];
            case PieceType::BISHOP: return bishopTable[row][col];
            case PieceType::ROOK: return rookTable[row][col];
            case PieceType::QUEEN: return queenTable[row][col];
            case PieceType::KING: return kingTable[row][col];
            default: return 0;
        }
    }

}
int evaluateIsolatedPawns(const Board& board, PieceColor color) {
    bool pawnsOnFile[8] = {false};
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = board.getPiece(row, col);
            if (piece && piece->getColor() == color && piece->getType() == PieceType::PAWN) {
                pawnsOnFile[col] = true;
            }
        }
    }
    int penalty = 0;
    for (int file = 0; file < 8; ++file) {
        if (pawnsOnFile[file]) {
            bool hasAdjacentPawn = false;
            if (file > 0 && pawnsOnFile[file - 1]) hasAdjacentPawn = true;
            if (file < 7 && pawnsOnFile[file + 1]) hasAdjacentPawn = true;
            if (!hasAdjacentPawn) {
                penalty += 15;
            }
        }
    }
    return penalty;
}

int evaluate(const Board& board) {
    int score = 0;
    int whiteKingRow = -1, whiteKingCol = -1, blackKingRow = -1, blackKingCol = -1;
    bool whiteCastled = false, blackCastled = false;

    // Center control bonus
    const int centerSquares[4][2] = { {3,3}, {3,4}, {4,3}, {4,4} }; // d4, e4, d5, e5
    int whiteCenter = 0, blackCenter = 0;
    int whiteDevelopedKnights = 0, blackDevelopedKnights = 0;
    int whiteDevelopedBishops = 0, blackDevelopedBishops = 0;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = board.getPiece(row, col);
            if (!piece) continue;

            int baseValue = pieceValue(piece->getType());
            int positionalValue = getPieceSquareValue(piece->getType(), row, col, piece->getColor());

            // Penalize knights on rim heavily
            if (piece->getType() == PieceType::KNIGHT) {
                if (row == 0 || row == 7 || col == 0 || col == 7) {
                    positionalValue -= 100; // Heavy penalty for knights on rim
                }
            }
            
            // Penalize early king moves in opening/middlegame
            if (piece->getType() == PieceType::KING) {
                // Check if there are still many pieces (opening/middlegame)
                int totalPieces = 0;
                for (int r = 0; r < 8; r++) {
                    for (int c = 0; c < 8; c++) {
                        if (board.getPiece(r, c)) totalPieces++;
                    }
                }
                if (totalPieces > 20) { // Still in opening/middlegame
                    bool onStartSquare = (piece->getColor() == PieceColor::WHITE && row == 7 && col == 4) ||
                                       (piece->getColor() == PieceColor::BLACK && row == 0 && col == 4);
                    if (!onStartSquare) {
                        positionalValue -= 200; // Penalty for king not on starting square
                    }
                }
            }

            // Center control
            for (int i = 0; i < 4; ++i) {
                if (row == centerSquares[i][0] && col == centerSquares[i][1]) {
                    if (piece->getColor() == PieceColor::WHITE) whiteCenter++;
                    else blackCenter++;
                }
            }

            // Piece development (knights/bishops off back rank)
            if (piece->getType() == PieceType::KNIGHT) {
                if (piece->getColor() == PieceColor::WHITE && row < 7) whiteDevelopedKnights++;
                if (piece->getColor() == PieceColor::BLACK && row > 0) blackDevelopedKnights++;
            }
            if (piece->getType() == PieceType::BISHOP) {
                if (piece->getColor() == PieceColor::WHITE && row < 7) whiteDevelopedBishops++;
                if (piece->getColor() == PieceColor::BLACK && row > 0) blackDevelopedBishops++;
            }

            if (piece->getType() == PieceType::KING) {
                if (piece->getColor() == PieceColor::WHITE) {
                    whiteKingRow = row; whiteKingCol = col;
                    // TODO: Detect castling more robustly if possible
                    if (col == 6 || col == 2) whiteCastled = true;
                } else {
                    blackKingRow = row; blackKingCol = col;
                    if (col == 6 || col == 2) blackCastled = true;
                }
            }

            if (piece->getColor() == PieceColor::WHITE)
                score += baseValue + positionalValue;
            else
                score -= baseValue + positionalValue;
        }
    }
    // Add center control and development bonuses
    score += 15 * whiteCenter;
    score -= 15 * blackCenter;
    score += 10 * whiteDevelopedKnights;
    score -= 10 * blackDevelopedKnights;
    score += 10 * whiteDevelopedBishops;
    score -= 10 * blackDevelopedBishops;

    score -= evaluateIsolatedPawns(board, PieceColor::WHITE);
    score += evaluateIsolatedPawns(board, PieceColor::BLACK);

    // King safety heuristics
    // Penalize king moves in opening/middlegame (not on starting square and not castled)
    // Bonus for castling
    // Only apply if queens are still on the board (not endgame)
    bool queensPresent = false;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = board.getPiece(row, col);
            if (piece && piece->getType() == PieceType::QUEEN) {
                queensPresent = true;
                break;
            }
        }
        if (queensPresent) break;
    }
    if (queensPresent) {
        // White king
        if (!whiteCastled && (whiteKingRow != 7 || whiteKingCol != 4))
            score -= 40; // Penalize early king moves
        if (whiteCastled)
            score += 30; // Bonus for castling
        // Black king
        if (!blackCastled && (blackKingRow != 0 || blackKingCol != 4))
            score += 40; // Penalize early king moves for black (from white's perspective)
        if (blackCastled)
            score -= 30; // Bonus for castling (from white's perspective)
    }

    return score;
}
