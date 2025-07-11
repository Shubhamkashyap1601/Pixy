#include "core/board.h"
#include "core/move_generator.h"
#include "ai/bot.h"
#include "ui/chess_ui.h"
#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <fstream>
#include <sstream>

class ChessGame {
private:
    Board board;
    Bot bot;
    std::vector<std::string> moveHistory;
    bool vsComputer;
    bool whiteIsHuman;
    
public:
    ChessGame() : vsComputer(true), whiteIsHuman(true) {
        board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }
    
    void run() {
        ChessUI::displayWelcome();
        
        while (true) {
            ChessUI::displayMainMenu();
            int choice = ChessUI::getMenuChoice();
            
            switch (choice) {
                case 1:
                    setupVsComputer();
                    playGame();
                    break;
                case 2:
                    setupTwoPlayer();
                    playGame();
                    break;
                case 3:
                    loadCustomPosition();
                    break;
                case 4:
                    showSettings();
                    break;
                case 5:
                    std::cout << "\nThanks for playing Pixy Chess! 👋\n";
                    return;
            }
        }
    }
    
private:
    void setupVsComputer() {
        ChessUI::clearScreen();
        std::cout << "\n** Play as White or Black?\n";
        std::cout << "1. White (you go first)\n";
        std::cout << "2. Black (computer goes first)\n";
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        vsComputer = true;
        whiteIsHuman = (choice == 1);
        
        board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        moveHistory.clear();
    }
    
    void setupTwoPlayer() {
        vsComputer = false;
        board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        moveHistory.clear();
    }
    
    void playGame() {
        ChessUI::clearScreen();
        
        while (true) {
            ChessUI::displayBoard(board);
            ChessUI::displayGameStatus(board);
            
            if (!moveHistory.empty() && moveHistory.size() <= 10) {
                ChessUI::displayMoveHistory(moveHistory);
            }
            
            GameResult result = board.getGameResult();
            if (result != GameResult::Ongoing) {
                ChessUI::displayGameResult(result, board.getCurrentTurn());
                std::cout << "\nPress Enter to return to menu...";
                std::cin.get();
                return;
            }
            
            bool isHumanTurn = !vsComputer || 
                             (board.getCurrentTurn() == PieceColor::WHITE && whiteIsHuman) ||
                             (board.getCurrentTurn() == PieceColor::BLACK && !whiteIsHuman);
            
            if (isHumanTurn) {
                if (!handleHumanMove()) {
                    return; // Player quit
                }
            } else {
                handleBotMove();
            }
            
            ChessUI::clearScreen();
        }
    }
    
    bool handleHumanMove() {
        std::string input = ChessUI::getMoveInput();
        
        if (input == "q" || input == "Q") {
            return false;
        }
        
        if (input == "history" || input == "h") {
            ChessUI::displayMoveHistory(moveHistory);
            return true;
        }
        
        int fromRow, fromCol, toRow, toCol;
        if (parseMove(input, fromRow, fromCol, toRow, toCol)) {
            if (board.movePiece(fromRow, fromCol, toRow, toCol)) {
                moveHistory.push_back(input);
                return true;
            } else {
                std::cout << "❌ Invalid move! Try again.\n";
            }
        } else {
            std::cout << "❌ Invalid format! Use format: e2 e4\n";
        }
        
        return true;
    }
    
    void handleBotMove() {
        ChessUI::displayThinking();
        
        try {
            // Adaptive depth based on game phase
            int searchDepth = calculateSearchDepth();
            
            Move bestMove = bot.findBestMove(board, searchDepth);
            
            // Convert move to string for history
            std::string moveStr = moveToString(bestMove);
            moveHistory.push_back(moveStr);
            
            board.movePiece(bestMove.fromRow, bestMove.fromCol, bestMove.toRow, bestMove.toCol);
            
            std::cout << "🤖 Bot played: " << moveStr << "\n";
            std::cout << "Press Enter to continue...";
            std::cin.get();
            
        } catch (const std::exception& e) {
            std::cout << "❌ Bot error: " << e.what() << "\n";
            std::cout << "Press Enter to return to menu...";
            std::cin.get();
        }
    }
    
    int calculateSearchDepth() {
        int totalPieces = 0;
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                if (board.getPiece(r, c)) totalPieces++;
            }
        }
        
        if (totalPieces > 28) return 3;      // Opening
        else if (totalPieces > 16) return 4; // Middlegame  
        else return 5;                       // Endgame
    }
    
    std::string moveToString(const Move& move) {
        char fromFile = 'a' + move.fromCol;
        char fromRank = '8' - move.fromRow;
        char toFile = 'a' + move.toCol;
        char toRank = '8' - move.toRow;
        
        return std::string(1, fromFile) + std::string(1, fromRank) + " " + 
               std::string(1, toFile) + std::string(1, toRank);
    }
    
    bool parseMove(const std::string& input, int& fromRow, int& fromCol, int& toRow, int& toCol) {
        if (input.length() != 5 || input[2] != ' ') return false;

        char fc = tolower(input[0]);
        char fr = input[1];
        char tc = tolower(input[3]);
        char tr = input[4];

        if (fc < 'a' || fc > 'h' || fr < '1' || fr > '8' ||
            tc < 'a' || tc > 'h' || tr < '1' || tr > '8') {
            return false;
        }

        fromCol = fc - 'a';
        fromRow = 8 - (fr - '0');
        toCol = tc - 'a';
        toRow = 8 - (tr - '0');

        return true;
    }
    
    void loadCustomPosition() {
        ChessUI::clearScreen();
        std::cout << "\n📁 Load Custom Position\n";
        std::cout << "Enter FEN string (or 'back' to return):\n";
        
        std::string fen;
        std::getline(std::cin, fen);
        
        if (fen == "back") return;
        
        try {
            board.loadFEN(fen);
            std::cout << "✅ Position loaded successfully!\n";
            ChessUI::displayBoard(board);
            std::cout << "Press Enter to continue...";
            std::cin.get();
        } catch (const std::exception& e) {
            std::cout << "❌ Invalid FEN: " << e.what() << "\n";
            std::cout << "Press Enter to continue...";
            std::cin.get();
        }
    }
    
    void showSettings() {
        ChessUI::clearScreen();
        std::cout << "\n⚙️  Settings\n";
        std::cout << "1. Bot difficulty: Adaptive (3-5 depth)\n";
        std::cout << "2. UI style: Unicode chess pieces\n";
        std::cout << "3. Features: Move history, game analysis\n";
        std::cout << "\nPress Enter to return...";
        std::cin.get();
    }
};

int main() {
    ChessGame game;
    game.run();
    return 0;
}
