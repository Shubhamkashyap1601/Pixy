#pragma once

#ifndef BOARD_H
#define BOARD_H

#include "piece.h"
#include "move.h"
#include <string>
#include <unordered_map>


enum class GameResult {
    Ongoing,
    Checkmate,
    FiftyMoveRule,
    ThreefoldRepetition,
    Stalemate
};

class Board {

    int halfmoveClock = 0; 
    std::unordered_map<std::string, int> positionHistory;
    public:
        Board();
        ~Board();
        void setupBoard();
        void printBoard() const;
        bool movePiece(int fromRow, int fromCol, int toRow, int toCol);
        Piece* getPiece(int row, int col);
        Piece* const (*getBoard() const)[8];
        Move getLastMove() const;
        bool isInCheck(PieceColor kingColor) const;
        bool isInCheckAfterMove(int fromRow, int fromCol, int toRow, int toCol, PieceColor kingColor) const;
        PieceColor getCurrentTurn() const;
        void setCurrentTurn(PieceColor color) { currentTurn = color; }
        Piece* (*getWritableBoard())[8];
        Board(const Board& other);              
        Board& operator=(const Board& other); 
        void loadFEN(const std::string& fen);
        bool hasKingMoved(PieceColor color);
        bool hasRookMoved(PieceColor color, bool kingside);
        void setKingMoved(PieceColor color);
        void setRookMoved(PieceColor color, bool kingside);
        bool isValidCastlingMove(int fromRow, int fromCol, int toRow, int toCol, PieceColor color);

        bool isCheckmate(PieceColor color) const;
        bool isStalemate(PieceColor) const;
        bool hasLegalMoves(PieceColor) const;
        void recordPosition();
        bool isThreefoldRepetition() const;
        void resetPositionHistory();
        GameResult getGameResult() const;
        std::string generatePositionKey() const;

    private:
        Piece* board[8][8];
        Move lastMove;
        bool hasWhiteKingMoved = false;
        bool hasBlackKingMoved = false;
        bool hasWhiteKingsideRookMoved = false;
        bool hasWhiteQueensideRookMoved = false;
        bool hasBlackKingsideRookMoved = false;
        bool hasBlackQueensideRookMoved = false;
        PieceColor currentTurn;
};

#endif
