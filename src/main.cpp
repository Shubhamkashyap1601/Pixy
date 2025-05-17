#include "core/board.h"
#include <iostream>
#include <string>
#include <cctype>
#include "core/move_generator.h"
#include <fstream>
#include <sstream>

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
    board.loadFEN("7k/5Q2/6K1/8/8/8/8/8 b - - 0 1");
    board.printBoard();

    GameResult result = board.getGameResult();
    if (result == GameResult::Checkmate) {
        std::cout << "Checkmate! " << (board.getCurrentTurn() == PieceColor::WHITE ? "Black" : "White") << " wins.\n";
        return 0;
    } else if (result == GameResult::Stalemate) {
        std::cout << "Stalemate! It's a draw.\n";
        return 0;
    }

    std::string input;
    while (true) {
        std::cout << "\nEnter your move (e.g., e2 e4) or 'q' to quit: ";
        std::getline(std::cin, input);

        if (input == "q" || input == "Q") {
            std::cout << "Goodbye!\n";
            break;
        }

        int fr, fc, tr, tc;
        if (parseMove(input, fr, fc, tr, tc)) {
            if (board.movePiece(fr, fc, tr, tc)) {
                board.printBoard();

                result = board.getGameResult();
                if (result == GameResult::Checkmate) {
                    std::cout << "Checkmate! " << (board.getCurrentTurn() == PieceColor::WHITE ? "Black" : "White") << " wins.\n";
                    break;
                } else if (result == GameResult::Stalemate) {
                    std::cout << "Stalemate! It's a draw.\n";
                    break;
                }
            } else {
                std::cout << "Invalid move (no piece at source).\n";
            }
        } else {
            std::cout << "Invalid input format.\n";
        }
    }

    return 0;
}