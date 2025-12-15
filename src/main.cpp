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
#include "engine/logger/logger.h"
#include <string>
#include <future>
#include "engine/tables/zobrist.h"

using namespace std;

const int TILE_SIZE = 80;
const int BOARD_SIZE = 8;

Move runEngineAsync(Board boardCopy, int difficultyLevel) {

    int maxDepthAllowed = 64;
    int timeLimitMs = 4000;


    // AI Difficulty Settings
    // 1 - Easy (2 depth, instant, 25% random blunder)
    // 2 - Medium (4 depth, 0.5s per move)
    // 3 - Hard (64 depth, 4s per move)
    switch (difficultyLevel) {
    case 1: // Easy
        maxDepthAllowed = 2; // Only thinks 2 moves ahead
        timeLimitMs = 10;    // Plays instantly
        break;
    case 2: // Medium
        maxDepthAllowed = 4; // 4 moves ahead
        timeLimitMs = 500;   // Half a second max per move
        break;
    case 3: // Hard
        maxDepthAllowed = 64; // Hikaru Nakamura level
        timeLimitMs = 4000;   // 4 seconds max per move
        break;
    }
    int aiSide = -1; // AI is playing black always, to change later
    // Get legal moves for AI side
    auto moves = legalMoves(boardCopy, to01(aiSide));
    std::string msg = "AI moves generated: " + std::to_string(moves.size());
    LOG(msg);

    // Checkmate / Stalemate
    if (moves.empty()) {
        // Two options if no moves
        if (isInCheck(boardCopy, to01(aiSide))) {
            std::string msg = "Checkmate! Game over. ";
            LOG(msg);
            return Move{ {-1,-1}, {-1,-1}, nullptr, nullptr }; // Indicate game over
        }
        else {
            std::string msg = "Stalemate! Draw. ";
            LOG(msg);
            return Move{ {-1,-1}, {-1,-1}, nullptr, nullptr }; // Indicate game over
        }
    }

    // Sort here to improve alpha-beta efficiency
    orderMoves(moves);

    sf::Clock clock;
    Move bestMoveOfAll = moves[0]; // Best move
    bool timeUp = false;

    for (int currentDepth = 1; currentDepth <= maxDepthAllowed; ++currentDepth) {

        int alpha = -INF;
        int beta = INF;
        Move bestMoveThisDepth = moves[0];
        int bestScoreThisDepth = -1000000;

        for (int i = 0; i < moves.size(); ++i) {
            // Primitive comparison
            if (moves[i].from.row == bestMoveOfAll.from.row &&
                moves[i].from.col == bestMoveOfAll.from.col &&
                moves[i].to.row == bestMoveOfAll.to.row &&
                moves[i].to.col == bestMoveOfAll.to.col) {

                std::swap(moves[0], moves[i]); // Put it first
                break;
            }
        }
        // ROOT SEARCH
        for (auto& move : moves) {
            if (clock.getElapsedTime().asMilliseconds() > timeLimitMs) {
                timeUp = true;
                break; // Times up
            }
            Undo undo;
            applyMove(boardCopy, move, undo);

            // Remember negation
            int score = -negamax(boardCopy, currentDepth - 1, -beta, -alpha, -aiSide);

            undoMove(boardCopy, move, undo);

            if (score > bestScoreThisDepth) {
                bestScoreThisDepth = score;
                bestMoveThisDepth = move;
            }

            if (score > alpha) {
                alpha = score;
                // Optional
            std:string msg = "AI found better move " + std::to_string(currentDepth) +
                " Score: " + std::to_string(score) +
                " Move: (" + std::to_string(move.from.row) + "," + std::to_string(move.from.col) +
                ") -> (" + std::to_string(move.to.row) + "," + std::to_string(move.to.col) + ")";
            LOG(msg);
            }
        }
        if (timeUp) {
            std::string msg = "AI time limit reached at depth " + std::to_string(currentDepth);
            LOG(msg);
            break; // Exit depth loop
        }
        bestMoveOfAll = bestMoveThisDepth;

        if (difficultyLevel == 1 && moves.size() > 1) {
            // Easy level blunder simulation, 25% chance to make a random move so it's easier for the player
            if (rand() % 4 == 0) {
                int randomIdx = rand() % moves.size();
                bestMoveOfAll = moves[randomIdx];
                std::string msg = "AI blundered and picked a random move. Easy difficulty only";
                LOG(msg);
            }
        }
        std::string msg = "AI completed depth " + std::to_string(currentDepth) +
            " Best score: " + std::to_string(bestScoreThisDepth) +
            " Move: (" + std::to_string(bestMoveThisDepth.from.row) + "," + std::to_string(bestMoveThisDepth.from.col) +
            ") -> (" + std::to_string(bestMoveThisDepth.to.row) + "," + std::to_string(bestMoveThisDepth.to.col) + ")";
        LOG(msg);

        if (bestScoreThisDepth > 90000) break; // Mate found, no need to search deeper
        if (currentDepth >= maxDepthAllowed) {
            break;
        }
    }
	return bestMoveOfAll;
}
    
int main() {
	//Engine multithreading init
	std::future<Move> engineFuture; // Future for engine move
	bool isEngineThinking = false;  // AI thinking flag
    int difficultyLevel = 2; // 1-Easy, 2-Medium, 3-Hard
	initZobrist(); // Initialize Zobrist hashing


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

	board.computeZobristHash();
	board.positionHistory.push_back(board.zobristKey);


    int currentPlayer = 0; // 0 = białe, 1 = czarne
    Position selected = { -1, -1 };
    Piece* selectedPiece = nullptr;

	std::string msg = "Game started. White to move first.";
	LOG(msg);
	msg = "Initial Zobrist Hash: " + std::to_string(board.zobristKey);
	LOG(msg);

    if (board.zobristKey == 0) {
		std::string msg = "Zobrist Hash initialized to zero, error";
		LOG(msg);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (!isEngineThinking && currentPlayer == 0) {
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
                            board.computeZobristHash();
							board.positionHistory.push_back(board.zobristKey);
                            currentPlayer = 1 - currentPlayer;
                        }

                        selectedPiece = nullptr;
                        selected = { -1, -1 };
                    }
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
					std::string msg = "Checkmate! Game over. ";
					LOG(msg);
                }
                else {
					std::string msg = "Stalemate! Draw. ";
					LOG(msg);
                }

				// Game over handling here
                window.display();
                continue; // or break
            }
		}


        if (currentPlayer == 1) // AI MOVE
        {
			// Engine async handling start
            if (!isEngineThinking) {
                std::string msg = "AI is thinking...";
				LOG(msg);
				isEngineThinking = true;
                engineFuture = std::async(std::launch::async, runEngineAsync, board, difficultyLevel);
            }
			// Check if engine is done
            if (engineFuture.valid() && engineFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                // Best move get
                Move bestMoveOfAll = engineFuture.get();
                std::string msg = "AI has made its move.";
                LOG(msg);
                if (bestMoveOfAll.pieceMoved == nullptr) {
                    //Game over?
                    std::string msg = "Game over detected from AI move.";
                    LOG(msg);

                    // Game over handling here
                    window.display();
                    continue; // or break
                }
                else {
                    Position from = bestMoveOfAll.from;
                    Position to = bestMoveOfAll.to;
                    Piece* realPiece = board.getPieceAt(from);
                    Piece* captured = board.getPieceAt(to);

                    if (realPiece) {
                        board.movePiece(from, to, realPiece);
                        if (captured) delete captured;
                        if (realPiece->getSymbol() == 'P' && (to.row == 0 || to.row == 7)) {
                            char newSymbol = 'Q'; // Default to Queen
                            board.promotePawn(board, to, newSymbol, currentPlayer);
                        }
                        board.computeZobristHash();
                        board.positionHistory.push_back(board.zobristKey);
                        currentPlayer = 0;
                    }
                    else {
                        std::string msg = "Critical error. AI Move execution failed: No piece at from position (" +
                            std::to_string(from.row) + "," + std::to_string(from.col) + ")";
                        LOG(msg);
                        currentPlayer = 0;
                    }

            // Give move back
			isEngineThinking = false;
                }
            }
        }
        window.display();
    }

    return 0;
}
