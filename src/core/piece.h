#pragma once

enum class PieceColor { WHITE, BLACK };
enum class PieceType { PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, NONE };

class Piece {
protected:
    PieceColor color;
    bool hasMoved;
public:
    Piece(PieceColor color) : color(color) {}
    virtual ~Piece();

    PieceColor getColor() const { return color; }
    virtual PieceType getType() const = 0; 
    virtual bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, Piece* board[8][8]) const = 0;
    virtual Piece* clone() const = 0;
    bool isWhite() const { return color == PieceColor::WHITE; }
};
