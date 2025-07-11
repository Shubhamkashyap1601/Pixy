#include "chess_ui.h"
#include <iostream>
#include <iomanip>
#include <cctype>

void ChessUI::displayMainMenu() {
    clearScreen();
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║            ** PIXY CHESS **          ║\n";
    std::cout << "╠══════════════════════════════════════╣\n";
    std::cout << "║                                      ║\n";
    std::cout << "║  1. Play vs Computer                 ║\n";
    std::cout << "║  2. Two Player Game                  ║\n";
    std::cout << "║  3. Load Position (FEN)              ║\n";
    std::cout << "║  4. Settings                         ║\n";
    std::cout << "║  5. Exit                             ║\n";
    std::cout << "║                                      ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    std::cout << "\nEnter your choice (1-5): ";
}

int ChessUI::getMenuChoice() {
    int choice;
    while (!(std::cin >> choice) || choice < 1 || choice > 5) {
        std::cout << "Invalid choice! Please enter 1-5: ";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
    std::cin.ignore(); // Clear the newline
    return choice;
}

void ChessUI::displayBoard(const Board& board, bool showCoordinates) {
    std::cout << "\n";
    
    if (showCoordinates) {
        std::cout << "    a   b   c   d   e   f   g   h\n";
        std::cout << "  ┌───┬───┬───┬───┬───┬───┬───┬───┐\n";
    }
    
    for (int row = 0; row < 8; row++) {
        if (showCoordinates) {
            std::cout << (8 - row) << " │";
        }
        
        for (int col = 0; col < 8; col++) {
            Piece* piece = board.getPiece(row, col);
            char symbol = getPieceSymbol(piece);
            
            // Add background color for better visibility
            bool isLight = (row + col) % 2 == 0;
            if (isLight) {
                std::cout << " " << symbol << " ";
            } else {
                std::cout << " " << symbol << " ";
            }
            
            if (showCoordinates && col < 7) {
                std::cout << "│";
            }
        }
        
        if (showCoordinates) {
            std::cout << "│ " << (8 - row);
        }
        std::cout << "\n";
        
        if (showCoordinates && row < 7) {
            std::cout << "  ├───┼───┼───┼───┼───┼───┼───┼───┤\n";
        }
    }
    
    if (showCoordinates) {
        std::cout << "  └───┴───┴───┴───┴───┴───┴───┴───┘\n";
        std::cout << "    a   b   c   d   e   f   g   h\n";
    }
}

char ChessUI::getPieceSymbol(const Piece* piece) {
    if (!piece) return '.';
    
    // ASCII chess symbols for better compatibility
    switch (piece->getType()) {
        case PieceType::KING:
            return piece->getColor() == PieceColor::WHITE ? 'K' : 'k';
        case PieceType::QUEEN:
            return piece->getColor() == PieceColor::WHITE ? 'Q' : 'q';
        case PieceType::ROOK:
            return piece->getColor() == PieceColor::WHITE ? 'R' : 'r';
        case PieceType::BISHOP:
            return piece->getColor() == PieceColor::WHITE ? 'B' : 'b';
        case PieceType::KNIGHT:
            return piece->getColor() == PieceColor::WHITE ? 'N' : 'n';
        case PieceType::PAWN:
            return piece->getColor() == PieceColor::WHITE ? 'P' : 'p';
        default:
            return '?';
    }
}

void ChessUI::displayGameStatus(const Board& board) {
    std::cout << "\n";
    std::cout << "┌─────────────────────────────────────┐\n";
    std::cout << "│ Turn: " << (board.getCurrentTurn() == PieceColor::WHITE ? "White (K)" : "Black (k)") << std::setw(21) << "│\n";
    
    GameResult result = board.getGameResult();
    if (result != GameResult::Ongoing) {
        std::cout << "│ Status: ";
        switch (result) {
            case GameResult::Checkmate:
                std::cout << "CHECKMATE!";
                break;
            case GameResult::Stalemate:
                std::cout << "STALEMATE!";
                break;
            case GameResult::FiftyMoveRule:
                std::cout << "50-Move Rule Draw";
                break;
            case GameResult::ThreefoldRepetition:
                std::cout << "Threefold Repetition";
                break;
            default:
                break;
        }
        std::cout << std::setw(20) << "│\n";
    }
    
    std::cout << "└─────────────────────────────────────┘\n";
}

std::string ChessUI::getMoveInput() {
    std::cout << "\n>> Enter move (e.g., e2 e4) or 'q' to quit: ";
    std::string input;
    std::getline(std::cin, input);
    return input;
}

void ChessUI::displayMoveHistory(const std::vector<std::string>& moveHistory) {
    if (moveHistory.empty()) return;
    
    std::cout << "\n┌─── Move History ───┐\n";
    for (size_t i = 0; i < moveHistory.size(); i++) {
        if (i % 2 == 0) {
            std::cout << "│ " << (i/2 + 1) << ". " << std::setw(6) << moveHistory[i];
        } else {
            std::cout << " " << std::setw(6) << moveHistory[i] << " │\n";
        }
    }
    if (moveHistory.size() % 2 == 1) {
        std::cout << std::setw(12) << "│\n";
    }
    std::cout << "└────────────────────┘\n";
}

void ChessUI::clearScreen() {
    // Cross-platform clear screen
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void ChessUI::displayWelcome() {
    clearScreen();
    std::cout << "\n";
    std::cout << "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n";
    std::cout << "░                                     ░\n";
    std::cout << "░        ♔ Welcome to PIXY! ♛        ░\n";
    std::cout << "░     Advanced Chess Engine v1.0     ░\n";
    std::cout << "░                                     ░\n";
    std::cout << "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n";
    std::cout << "\nPress Enter to continue...";
    std::cin.get();
}

void ChessUI::displayGameResult(GameResult result, PieceColor currentTurn) {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════╗\n";
    std::cout << "║              GAME OVER!               ║\n";
    std::cout << "╠═══════════════════════════════════════╣\n";
    
    switch (result) {
        case GameResult::Checkmate:
            std::cout << "║   CHECKMATE! " << (currentTurn == PieceColor::WHITE ? "Black" : "White") << " wins!";
            std::cout << std::setw(12) << "║\n";
            break;
        case GameResult::Stalemate:
            std::cout << "║           STALEMATE - Draw!           ║\n";
            break;
        case GameResult::FiftyMoveRule:
            std::cout << "║       Draw by 50-move rule!           ║\n";
            break;
        case GameResult::ThreefoldRepetition:
            std::cout << "║    Draw by threefold repetition!      ║\n";
            break;
        default:
            std::cout << "║              Game ended!              ║\n";
    }
    
    std::cout << "╚═══════════════════════════════════════╝\n";
}

void ChessUI::displayThinking() {
    std::cout << "\n🤖 Bot is analyzing position";
    std::cout.flush();
    for (int i = 0; i < 3; i++) {
        std::cout << ".";
        std::cout.flush();
    }
    std::cout << "\n";
}

void ChessUI::displayBotStats(int nodes, int ttHits, int evaluation) {
    std::cout << "┌─── Bot Statistics ───┐\n";
    std::cout << "│ Nodes: " << std::setw(12) << nodes << " │\n";
    std::cout << "│ Cache hits: " << std::setw(8) << ttHits << " │\n";
    std::cout << "│ Evaluation: " << std::setw(8) << evaluation << " │\n";
    std::cout << "└───────────────────────┘\n";
}
