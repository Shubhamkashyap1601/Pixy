#include "move_generator.h"
#include <iostream>
#include <vector>

int MoveGenerator::countAllLegalMoves(const Board& board) {
    int count = 0;
    PieceColor currentTurn = board.getCurrentTurn();
    Piece* const (*grid)[8] = board.getBoard();


    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            Piece* piece = grid[fromRow][fromCol];
            if (!piece || piece->getColor() != currentTurn)
                continue;

            for (int toRow = 0; toRow < 8; ++toRow) {
                for (int toCol = 0; toCol < 8; ++toCol) {
                    if (piece->isValidMove(fromRow, fromCol, toRow, toCol, const_cast<Piece* (*)[8]>(grid))) {
                        Board temp = board;
                        // Piece* captured = temp.getPiece(toRow, toCol);
                        temp.getWritableBoard()[toRow][toCol] = temp.getPiece(fromRow, fromCol);
                        temp.getWritableBoard()[fromRow][fromCol] = nullptr;


                        if (!temp.isInCheck(currentTurn)) {
                            ++count;
                            std::cout << "Valid move: ";
                            std::cout << (char)('a' + fromCol) << 8 - fromRow << " to ";
                            std::cout << (char)('a' + toCol) << 8 - toRow << std::endl;
                        }
                    }
                }
            }
        }
    }

    return count;
}

std::vector<Move> MoveGenerator::generateLegalMoves(const Board& board, PieceColor color) {
    std::vector<Move> legalMoves;

    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            Piece* const (*grid)[8] = board.getBoard();
            Piece* piece = grid[fromRow][fromCol];
            if (!piece || piece->getColor() != color) continue;

            for (int toRow = 0; toRow < 8; ++toRow) {
                for (int toCol = 0; toCol < 8; ++toCol) {
                    if (fromRow == toRow && fromCol == toCol) continue;

                    if (piece->isValidMove(fromRow, fromCol, toRow, toCol, const_cast<Piece* (*)[8]>(board.getBoard()))) {
                        // Make a copy of the board to simulate the move
                        Board tempBoard = board;
                        if (tempBoard.movePiece(fromRow, fromCol, toRow, toCol)) {
                            // If king is not in check after move, it's legal
                            if (!tempBoard.isInCheck(color)) {
                                legalMoves.emplace_back(fromRow, fromCol, toRow, toCol, piece);
                            }
                        }
                    }
                }
            }
        }
    }

    return legalMoves;
}
