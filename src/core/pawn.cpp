#include "pawn.h"

Pawn::Pawn(PieceColor color) : Piece(color) {}

Piece *Pawn::clone() const
{
    return new Pawn(*this);
}

PieceType Pawn::getType() const {
    return PieceType::PAWN;
}

bool Pawn::isValidMove(int fromRow, int fromCol, int toRow, int toCol, Piece* board[8][8]) const {
    int direction = (color == PieceColor::WHITE) ? -1 : 1;
    int startRow = (color == PieceColor::WHITE) ? 6 : 1;

    int rowDiff = toRow - fromRow;
    int colDiff = toCol - fromCol;

    if (colDiff == 0) {
        if (rowDiff == direction && board[toRow][toCol] == nullptr) return true;
        if (fromRow == startRow && rowDiff == 2 * direction &&
            board[fromRow + direction][fromCol] == nullptr &&
            board[toRow][toCol] == nullptr) return true;
    }

    if (abs(colDiff) == 1 && rowDiff == direction &&
        board[toRow][toCol] != nullptr &&
        board[toRow][toCol]->getColor() != this->getColor()) {
        return true;
    }

    return false;
}

bool Pawn::canEnPassant(int fromRow, int fromCol, int toRow, int toCol, Piece* (*board)[8], const Move& lastMove) const
{
// 1. Direction: White moves up (-1), Black moves down (+1)
int dir = (getColor() == PieceColor::WHITE) ? -1 : 1;

// 2. Must move exactly one square diagonally
if (toRow - fromRow != dir || abs(toCol - fromCol) != 1) {
std::cout << "Invalid en passant direction\n";
return false;
}

// 3. Destination square must be empty
if (board[toRow][toCol] != nullptr) {
std::cout << "En passant target square not empty\n";
return false;
}

// 4. Last move must exist and be an opponent pawn
Piece* lastPawn = lastMove.pieceMoved;
if (!lastPawn ||
lastPawn->getColor() == getColor() ||
lastPawn->getType()  != PieceType::PAWN) {
std::cout << "Last move wasn't an opponent pawn\n";
return false;
}

// 5. That pawn must have moved two squares
if (abs(lastMove.toRow - lastMove.fromRow) != 2) {
std::cout << "Last pawn did not move two squares\n";
return false;
}

// 6. And it must now be adjacent to this pawn
//    (same row as 'fromRow', in column 'toCol')
if (lastMove.toRow != fromRow || lastMove.toCol != toCol) {
std::cout << "Last pawn not adjacent for en passant\n";
return false;
}

std::cout << "En passant is valid!\n";
return true;
}
