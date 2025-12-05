#include <SFML/Graphics.hpp>
#include <iostream>
#include "Board.h"
#include "Piece.h"
#include "Pawn.h"
#include "Rook.h"
#include "kNight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"
#include "engine/engine.h"
#include "engine/val.h"
#include "engine/deepcopy.h"
#include "engine/evalpos.cpp"

using namespace std;

const int TILE_SIZE = 80;
const int BOARD_SIZE = 8;

int main() {
    sf::RenderWindow window(sf::VideoMode(BOARD_SIZE * TILE_SIZE, BOARD_SIZE * TILE_SIZE), "CHESS");


    //tekstury figur
    sf::Texture textures[12];
    textures[0].loadFromFile("pieces/White_Pawn.png");
    textures[1].loadFromFile("pieces/White_Rook.png");
    textures[2].loadFromFile("pieces/White_Knight.png");
    textures[3].loadFromFile("pieces/White_Bishop.png");
    textures[4].loadFromFile("pieces/White_Queen.png");
    textures[5].loadFromFile("pieces/White_King.png");
    textures[6].loadFromFile("pieces/Black_Pawn.png");
    textures[7].loadFromFile("pieces/Black_Rook.png");
    textures[8].loadFromFile("pieces/Black_Knight.png");
    textures[9].loadFromFile("pieces/Black_Bishop.png");
    textures[10].loadFromFile("pieces/Black_Queen.png");
    textures[11].loadFromFile("pieces/Black_King.png");
    Board board;

    // Białe
    board.placePiece(new Rook(0, 'R', { 0, 0 }));
    board.placePiece(new Knight(0, 'N', { 0, 1 }));
    board.placePiece(new Bishop(0, 'B', { 0, 2 }));
    board.placePiece(new Queen(0, 'Q', { 0, 3 }));
    board.placePiece(new King(0, 'K', { 0, 4 }));
    board.placePiece(new Bishop(0, 'B', { 0, 5 }));
    board.placePiece(new Knight(0, 'N', { 0, 6 }));
    board.placePiece(new Rook(0, 'R', { 0, 7 }));
    for (int i = 0; i < BOARD_SIZE; i++) board.placePiece(new Pawn(0, 'P', { 1, i }));

    // Czarne
    board.placePiece(new Rook(1, 'R', { 7, 0 }));
    board.placePiece(new Knight(1, 'N', { 7, 1 }));
    board.placePiece(new Bishop(1, 'B', { 7, 2 }));
    board.placePiece(new Queen(1, 'Q', { 7, 3 }));
    board.placePiece(new King(1, 'K', { 7, 4 }));
    board.placePiece(new Bishop(1, 'B', { 7, 5 }));
    board.placePiece(new Knight(1, 'N', { 7, 6 }));
    board.placePiece(new Rook(1, 'R', { 7, 7 }));
    for (int i = 0; i < BOARD_SIZE; i++) board.placePiece(new Pawn(1, 'P', { 6, i }));

    int currentPlayer = 0; // 0 = białe, 1 = czarne
    Position selected = { -1, -1 };
    Piece* selectedPiece = nullptr;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int col = event.mouseButton.x / TILE_SIZE;
                int row = 7 - (event.mouseButton.y / TILE_SIZE);

                if (!selectedPiece) {
                    // wybierz figurę
                    selectedPiece = board.getPieceAt({ row, col });
                    if (selectedPiece && selectedPiece->getColor() == currentPlayer)
                        selected = { row, col };
                    else
                        selectedPiece = nullptr;
                }
                else {
                    Position target = { row, col };
                    Piece* captured = board.getPieceAt(target);

                    if (board.movePiece(selected, target, selectedPiece)) {
                        if (captured) delete captured;
						if (selectedPiece->getSymbol() == 'P' && (target.row == 0 || target.row == 7)) {
							char newSymbol = 'Q'; // Default to Queen
							board.promotePawn(board, target, newSymbol, currentPlayer);
						}
                        currentPlayer = 1 - currentPlayer;
                    }

                    selectedPiece = nullptr;
                    selected = { -1, -1 };
                }
            }
        }

        // plansza
        window.clear();
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                sf::RectangleShape square(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                square.setPosition(c * TILE_SIZE, (7 - r) * TILE_SIZE);
                if ((r + c) % 2 == 0) square.setFillColor(sf::Color(240, 217, 181));
                else square.setFillColor(sf::Color(181, 136, 99));
                window.draw(square);
            }
        }

        // figury
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                Piece* piece = board.getPieceAt({ r, c });
                if (!piece) continue;

                sf::Sprite sprite;
                int color = piece->getColor();
                char symbol = piece->getSymbol();

                if (color == 0) { // białe
                    switch (symbol) {
                    case 'P': sprite.setTexture(textures[0]); break;
                    case 'R': sprite.setTexture(textures[1]); break;
                    case 'N': sprite.setTexture(textures[2]); break;
                    case 'B': sprite.setTexture(textures[3]); break;
                    case 'Q': sprite.setTexture(textures[4]); break;
                    case 'K': sprite.setTexture(textures[5]); break;
                    }
                }
                else { // czarne
                    switch (symbol) {
                    case 'P': sprite.setTexture(textures[6]); break;
                    case 'R': sprite.setTexture(textures[7]); break;
                    case 'N': sprite.setTexture(textures[8]); break;
                    case 'B': sprite.setTexture(textures[9]); break;
                    case 'Q': sprite.setTexture(textures[10]); break;
                    case 'K': sprite.setTexture(textures[11]); break;
                    }
                }

                sprite.setPosition(c * TILE_SIZE, (7 - r) * TILE_SIZE);
                sprite.setScale(TILE_SIZE / sprite.getLocalBounds().width, TILE_SIZE / sprite.getLocalBounds().height);
                window.draw(sprite);
            }
        }
		if (currentPlayer == 0) // PLAYER MOVE
		{
            OwnedBoard ob(board);
            Board& copy = ob.board;

            auto moves = legalMoves(copy, to01(currentPlayer));
            if (moves.empty()) {
                // Two options if no moves
                if (isInCheck(copy, to01(currentPlayer))) {
                    std::cout << "SZACH MAT! Koniec gry." << std::endl;
                }
                else {
                    std::cout << "PAT! Remis." << std::endl;
                }

				// Game over handling here
                window.display();
                continue; // or break
            }
		}


        if (currentPlayer == 1) // AI MOVE
        {
            OwnedBoard ob(board);
            Board& copy = ob.board;

            int aiSide = -1; // AI is playing black always, to change later

            //Generate moves
            auto moves = legalMoves(copy, to01(aiSide));

            std::cout << "--- RUCHY AI ---" << std::endl;
            for (auto& m : moves) {
                if (toupper(m.pieceMoved->getSymbol()) == 'P') {
                    std::cout << "Pion: " << m.from.row << "," << m.from.col
                        << " -> " << m.to.row << "," << m.to.col << std::endl;
                }
            }

			// Checkmate / Stalemate
            if (moves.empty()) {
                // Two options if no moves
                if (isInCheck(copy, to01(aiSide))) {
                    std::cout << "SZACH MAT! Koniec gry." << std::endl;
                }
                else {
                    std::cout << "PAT! Remis." << std::endl;
                }

                // Game over handling here
                window.display();
                continue; // or break
            }

			// Sort here to improve alpha-beta efficiency
            orderMoves(moves);

			int alpha = -INF; // from val.h
            int beta = INF;   
            Move bestMove = moves[0]; // Default if all moves are terrible

            // ROOT SEARCH
            for (auto& move : moves) {
                Undo undo;
                applyMove(copy, move, undo);

                // Remember negation
                int score = -negamax(copy, depth - 1, -beta, -alpha, -aiSide);

                undoMove(copy, move, undo);

                if (score > alpha) {
                    alpha = score;
                    bestMove = move;
                    // Optional
                     std::cout << "AI found better move" << score << std::endl;
                }
            }
            Position from = bestMove.from;
            Position to = bestMove.to;

            Piece* realPiece = board.getPieceAt(from);
            Piece* captured = board.getPieceAt(to);

            // REAL MOVE
            if (realPiece) {
                // Update board state
                board.movePiece(from, to, realPiece);

				// Cleanup captured piece
                if (captured) {
                    delete captured;
                }
            }
            else {
                std::cout << "BŁĄD KRYTYCZNY: AI chce ruszyć figurą, której nie ma na planszy!" << std::endl;
            }

            // Give move back
            currentPlayer = 0;
        }

        window.display();
    }

    return 0;
}
