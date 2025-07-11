#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "../core/board.h"

class ChessUI {
public:
    static void displayMainMenu();
    static int getMenuChoice();
    static void displayBoard(const Board& board, bool showCoordinates = true);
    static void displayGameStatus(const Board& board);
    static std::string getMoveInput();
    static void displayMoveHistory(const std::vector<std::string>& moveHistory);
    static void clearScreen();
    static void displayWelcome();
    static void displayGameResult(GameResult result, PieceColor currentTurn);
    static void displayThinking();
    static void displayBotStats(int nodes, int ttHits, int evaluation);
    
private:
    static char getPieceSymbol(const Piece* piece);
    static std::string getColorCode(PieceColor color);
    static void displayBoardFrame();
    static void displayCoordinates();
};
