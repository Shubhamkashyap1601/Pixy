#include "core/board.h"
#include <iostream>
#include <string>
#include <cctype>
#include "core/move_generator.h"
#include <fstream>
#include <sstream>
#include "ai/bot.h"

// Function to test loading FEN from a file
void testFENLoading(const std::string& filename) {
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string fen;
    Board board;

    while (std::getline(inputFile, fen)) {
        if (fen.empty()) continue;

        std::cout << "Loading FEN: " << fen << std::endl;

      
        board.loadFEN(fen);
        board.printBoard();
        int legalMoves = MoveGenerator::countAllLegalMoves(board);
        std::cout << "Number of legal moves: " << legalMoves << std::endl;

        std::cout << "-----------------------------" << std::endl;
    }

    inputFile.close();
}

bool parseMove(const std::string& input, int& fromRow, int& fromCol, int& toRow, int& toCol) {
    if (input.length() != 5 || input[2] != ' ') return false;

    char fc = tolower(input[0]);
    char fr = input[1];
    char tc = tolower(input[3]);
    char tr = input[4];

    std::cout << "Parsed input: " << input << std::endl;
    std::cout << "From: " << fc << fr << ", To: " << tc << tr << std::endl;

    if (fc < 'a' || fc > 'h' || fr < '1' || fr > '8' ||
        tc < 'a' || tc > 'h' || tr < '1' || tr > '8') {
        std::cout << "Invalid coordinates detected!" << std::endl;
        return false;
    }

    fromCol = fc - 'a';
    fromRow = 8 - (fr - '0');
    toCol = tc - 'a';
    toRow = 8 - (tr - '0');

    std::cout << "From row: " << fromRow << ", From col: " << fromCol << std::endl;
    std::cout << "To row: " << toRow << ", To col: " << toCol << std::endl;

    return true;
}



int main() {
    Board board;
    Bot bot;

    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // board.loadFEN("rnb1kbnr/pp1ppp1p/2p3p1/q7/2PP4/2N5/PP2PPPP/R1BQKBNR w KQkq - 2 4");
    board.printBoard();

    std::string input;
    while (true) {
        GameResult result = board.getGameResult();
        if (result == GameResult::Checkmate) {
            std::cout << "Checkmate! " << (board.getCurrentTurn() == PieceColor::WHITE ? "Black" : "White") << " wins.\n";
            break;
        } else if (result == GameResult::Stalemate) {
            std::cout << "Stalemate! It's a draw.\n";
            break;
        } else if (result == GameResult::FiftyMoveRule) {
            std::cout << "Draw by fifty-move rule.\n";
            break;
        } else if (result == GameResult::ThreefoldRepetition) {
            std::cout << "Draw by threefold repetition.\n";
            break;
        }

        if (board.getCurrentTurn() == PieceColor::WHITE) {
            std::cout << "\nEnter your move (e.g., e2 e4) or 'q' to quit: ";
            std::getline(std::cin, input);
            if (input == "q" || input == "Q") {
                std::cout << "Goodbye!\n";
                break;
            }

            int fr, fc, tr, tc;
            if (parseMove(input, fr, fc, tr, tc)) {
                if (!board.movePiece(fr, fc, tr, tc)) {
                    std::cout << "Invalid move.\n";
                }
            } else {
                std::cout << "Invalid input format.\n";
            }

        } else {
            std::cout << "Bot is thinking...\n";
            board.isBotPlaying = true;
            try {
                // Adaptive depth: deeper in endgame, shallower in opening for speed
                int searchDepth = 4;
                int totalPieces = 0;
                for (int r = 0; r < 8; r++) {
                    for (int c = 0; c < 8; c++) {
                        if (board.getPiece(r, c)) totalPieces++;
                    }
                }
                if (totalPieces > 28) searchDepth = 3; // Opening: faster search  
                else if (totalPieces < 12) searchDepth = 5; // Endgame: deeper search
                
                Move bestMove = bot.findBestMove(board, searchDepth);
                board.movePiece(bestMove.fromRow, bestMove.fromCol, bestMove.toRow, bestMove.toCol);
            } catch (const std::exception& e) {
                std::cout << "Bot failed to find move: " << e.what() << '\n';
                break;
            }
            board.isBotPlaying = false;
        }

        board.printBoard();
    }

    return 0;
}