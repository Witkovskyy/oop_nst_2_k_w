#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include "Board.h"
#include "Piece.h"
#include "Pawn.h"
#include "Rook.h"
#include "Knight.h"
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
const int SIDEBAR_WIDTH = 260; // Nieco szerszy panel dla pewności

// Funkcja centrująca tekst w poziomie względem panelu bocznego
void centerText(sf::Text& text, float y, float boardWidth, float sidebarWidth) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(boardWidth + sidebarWidth / 2.0f, y);
}

string formatTime(float timeInSeconds) {
    if (timeInSeconds < 0) timeInSeconds = 0;
    int minutes = (int)timeInSeconds / 60;
    int seconds = (int)timeInSeconds % 60;
    stringstream ss;
    ss << setfill('0') << setw(2) << minutes << ":" << setw(2) << seconds;
    return ss.str();
}

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
            std::string msg = "AI found better move " + std::to_string(currentDepth) +
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
    int difficultyLevel = 3; // 1-Easy, 2-Medium, 3-Hard
	initZobrist(); // Initialize Zobrist hashing

    //tekstury figur
    // --- 1. KONFIGURACJA CZASU (W KONSOLI) ---
    float timeLimitMinutes;
    cout << "--- USTAWIENIA GRY ---" << endl;
    cout << "Podaj czas na partie dla gracza (w minutach): ";
    if (!(cin >> timeLimitMinutes)) {
        timeLimitMinutes = 10;
        cout << "Bledne dane, ustawiono 10 minut." << endl;
    }

    // --- INICJALIZACJA SFML ---
    sf::RenderWindow window(sf::VideoMode(BOARD_SIZE * TILE_SIZE + SIDEBAR_WIDTH, BOARD_SIZE * TILE_SIZE), "CHESS - C++ Engine");

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        cout << "BLAD: Brak pliku arial.ttf w folderze projektu!" << endl;
    }

    // --- ELEMENTY UI ---

    // Tło panelu
    sf::RectangleShape sidebarBg(sf::Vector2f((float)SIDEBAR_WIDTH, (float)(BOARD_SIZE * TILE_SIZE)));
    sidebarBg.setPosition((float)(BOARD_SIZE * TILE_SIZE), 0);
    sidebarBg.setFillColor(sf::Color(40, 40, 40));

    // --- ZEGARY (Teraz identyczny styl dla obu) ---
    sf::Color timerBgColor(220, 220, 220); // Jasnoszary dla obu
    sf::Color timerTextColor = sf::Color::Black; // Czarny tekst dla obu

    // Zegar CZARNYCH (Góra)
    sf::RectangleShape blackTimerBox(sf::Vector2f(SIDEBAR_WIDTH - 40.f, 80.f));
    blackTimerBox.setPosition((float)(BOARD_SIZE * TILE_SIZE + 20), 50.f);
    blackTimerBox.setFillColor(timerBgColor);
    blackTimerBox.setOutlineThickness(3);
    blackTimerBox.setOutlineColor(sf::Color::Black); // Czarna obwódka

    // Zegar BIAŁYCH (Dół)
    sf::RectangleShape whiteTimerBox(sf::Vector2f(SIDEBAR_WIDTH - 40.f, 80.f));
    whiteTimerBox.setPosition((float)(BOARD_SIZE * TILE_SIZE + 20), (float)(BOARD_SIZE * TILE_SIZE - 130));
    whiteTimerBox.setFillColor(timerBgColor);
    whiteTimerBox.setOutlineThickness(3);
    whiteTimerBox.setOutlineColor(sf::Color::White); // Biała obwódka (jedyna różnica)

    // Teksty
    sf::Text whiteTimerText, blackTimerText, turnText, statusText, labelWhite, labelBlack;

    whiteTimerText.setFont(font); whiteTimerText.setCharacterSize(50); whiteTimerText.setFillColor(timerTextColor);
    blackTimerText.setFont(font); blackTimerText.setCharacterSize(50); blackTimerText.setFillColor(timerTextColor);

    turnText.setFont(font);       turnText.setCharacterSize(26); turnText.setFillColor(sf::Color::White);

    // Status text - mniejszy font, żeby się zmieścił
    statusText.setFont(font);     statusText.setCharacterSize(22); statusText.setFillColor(sf::Color(255, 80, 80)); statusText.setStyle(sf::Text::Bold);

    // Podpisy
    labelBlack.setFont(font); labelBlack.setString("GRACZ CZARNY"); labelBlack.setCharacterSize(18); labelBlack.setFillColor(sf::Color(180, 180, 180));
    labelBlack.setPosition((float)(BOARD_SIZE * TILE_SIZE + 25), 25.f);

    labelWhite.setFont(font); labelWhite.setString("GRACZ BIALY"); labelWhite.setCharacterSize(18); labelWhite.setFillColor(sf::Color(180, 180, 180));
    labelWhite.setPosition((float)(BOARD_SIZE * TILE_SIZE + 25), (float)(BOARD_SIZE * TILE_SIZE - 155));

    // --- ŁADOWANIE TEKSTUR I PLANSZY ---
    // (Tu bez zmian - skrótowo wklejone ładowanie)
    sf::Texture textures[12];
    textures[0].loadFromFile("pieces/White_Pawn.png"); textures[1].loadFromFile("pieces/White_Rook.png");
    textures[2].loadFromFile("pieces/White_Knight.png"); textures[3].loadFromFile("pieces/White_Bishop.png");
    textures[4].loadFromFile("pieces/White_Queen.png"); textures[5].loadFromFile("pieces/White_King.png");
    textures[6].loadFromFile("pieces/Black_Pawn.png"); textures[7].loadFromFile("pieces/Black_Rook.png");
    textures[8].loadFromFile("pieces/Black_Knight.png"); textures[9].loadFromFile("pieces/Black_Bishop.png");
    textures[10].loadFromFile("pieces/Black_Queen.png"); textures[11].loadFromFile("pieces/Black_King.png");

    Board board;
    // Białe
    board.placePiece(new Rook(0, 'R', { 0, 0 })); board.placePiece(new Knight(0, 'N', { 0, 1 }));
    board.placePiece(new Bishop(0, 'B', { 0, 2 })); board.placePiece(new Queen(0, 'Q', { 0, 3 }));
    board.placePiece(new King(0, 'K', { 0, 4 })); board.placePiece(new Bishop(0, 'B', { 0, 5 }));
    board.placePiece(new Knight(0, 'N', { 0, 6 })); board.placePiece(new Rook(0, 'R', { 0, 7 }));
    for (int i = 0; i < BOARD_SIZE; i++) board.placePiece(new Pawn(0, 'P', { 1, i }));
    // Czarne
    board.placePiece(new Rook(1, 'R', { 7, 0 })); board.placePiece(new Knight(1, 'N', { 7, 1 }));
    board.placePiece(new Bishop(1, 'B', { 7, 2 })); board.placePiece(new Queen(1, 'Q', { 7, 3 }));
    board.placePiece(new King(1, 'K', { 7, 4 })); board.placePiece(new Bishop(1, 'B', { 7, 5 }));
    board.placePiece(new Knight(1, 'N', { 7, 6 })); board.placePiece(new Rook(1, 'R', { 7, 7 }));
    for (int i = 0; i < BOARD_SIZE; i++) board.placePiece(new Pawn(1, 'P', { 6, i }));

	board.computeZobristHash();
	board.positionHistory.push_back(board.zobristKey);


    int currentPlayer = 0; // 0 = białe, 1 = czarne
    Position selected = { -1, -1 };
    Piece* selectedPiece = nullptr;
    bool gameOver = false;
    vector<Position> validMoves;

    float timeWhite = timeLimitMinutes * 60.0f;
    float timeBlack = timeLimitMinutes * 60.0f;
    sf::Clock dtClock;

	std::string msg = "Game started. White to move first.";
	LOG(msg);
	msg = "Initial Zobrist Hash: " + std::to_string(board.zobristKey);
	LOG(msg);

    if (board.zobristKey == 0) {
		std::string msg = "Zobrist Hash initialized to zero, error";
		LOG(msg);
    }

    while (window.isOpen()) {
        float dt = dtClock.restart().asSeconds();

        if (!gameOver) {
            if (currentPlayer == 0) timeWhite -= dt;
            else timeBlack -= dt;

            // Używamy \n w napisach, żeby były w wielu liniach
            if (timeWhite <= 0) {
                gameOver = true;
                statusText.setString("KONIEC CZASU!\nWygrywaja\nCZARNE");
                timeWhite = 0;
            }
            if (timeBlack <= 0) {
                gameOver = true;
                statusText.setString("KONIEC CZASU!\nWygrywaja\nBIALE");
                timeBlack = 0;
            }
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (!isEngineThinking && currentPlayer == 0) {
                if (!gameOver && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    if (event.mouseButton.x >= BOARD_SIZE * TILE_SIZE) continue;
                    int col = event.mouseButton.x / TILE_SIZE;
                    int row = 7 - (event.mouseButton.y / TILE_SIZE);
                    Position clickedPos = { row, col };

                    if (!selectedPiece) {
                        Piece* clickedPiece = board.getPieceAt(clickedPos);
                        if (clickedPiece && clickedPiece->getColor() == currentPlayer) {
                            selectedPiece = clickedPiece; selected = clickedPos;
                            validMoves.clear();
                            for (int r = 0; r < BOARD_SIZE; r++)
                                for (int c = 0; c < BOARD_SIZE; c++)
                                    if (board.isMoveSafe(selected, { r, c })) validMoves.push_back({ r, c });
                        }
                        else {
                            if (clickedPos.row == selected.row && clickedPos.col == selected.col) {
                                selectedPiece = nullptr; selected = { -1, -1 }; validMoves.clear();
                            }

                            else {
                                if (board.isMoveSafe(selected, clickedPos)) {
                                    Piece* captured = board.getPieceAt(clickedPos);
                                    Position target = { row, col };

                                    board.movePiece(selected, clickedPos, selectedPiece);
                                    if (captured) delete captured;
                                    if (selectedPiece->getSymbol() == 'P' && (target.row == 0 || target.row == 7)) {
                                        char newSymbol = 'Q'; // Default to Queen
                                        board.promotePawn(board, target, newSymbol, currentPlayer);
                                        currentPlayer = 1 - currentPlayer;

                                        if (board.isKingInCheck(currentPlayer)) {
                                            if (board.isCheckMate(currentPlayer)) {
                                                statusText.setString("MAT!\nWygrywa gracz:\n" + string(currentPlayer == 0 ? "CZARNY" : "BIALY"));
                                                gameOver = true;
                                            }
                                            else statusText.setString("SZACH!");
                                        }
                                        else statusText.setString("");
                                    }
                                    selectedPiece = nullptr; selected = { -1, -1 };
                                    validMoves.clear();
                                }
                            }
                        }
                    }
                }
            }

                    window.clear();
                    // Rysowanie planszy i figur
                    for (int r = 0; r < BOARD_SIZE; r++) {
                        for (int c = 0; c < BOARD_SIZE; c++) {
                            sf::RectangleShape square(sf::Vector2f((float)TILE_SIZE, (float)TILE_SIZE));
                            square.setPosition((float)(c * TILE_SIZE), (float)((7 - r) * TILE_SIZE));
                            if ((r + c) % 2 == 0) square.setFillColor(sf::Color(240, 217, 181));
                            else square.setFillColor(sf::Color(181, 136, 99));
                            if (selectedPiece && r == selected.row && c == selected.col) square.setFillColor(sf::Color(118, 150, 86));
                            Piece* p = board.getPieceAt({ r,c });
                            if (p && p->getSymbol() == 'K' && p->getColor() == currentPlayer && board.isKingInCheck(currentPlayer))
                                square.setFillColor(sf::Color(200, 50, 50));
                            window.draw(square);
                        }
                    }

                    for (Position pos : validMoves) {
                        float radius = TILE_SIZE / 6.0f;
                        sf::CircleShape dot(radius);
                        dot.setFillColor(sf::Color(100, 100, 100, 128));
                        dot.setOrigin(radius, radius);
                        dot.setPosition((float)(pos.col * TILE_SIZE) + (TILE_SIZE / 2.0f), (float)((7 - pos.row) * TILE_SIZE) + (TILE_SIZE / 2.0f));
                        if (!board.isEmpty(pos)) {
                            dot.setRadius(TILE_SIZE / 2.2f); dot.setOrigin(dot.getRadius(), dot.getRadius());
                            dot.setFillColor(sf::Color::Transparent); dot.setOutlineThickness(5);
                            dot.setOutlineColor(sf::Color(100, 100, 100, 128));
                        }
                        window.draw(dot);
                    }

                    for (int r = 0; r < BOARD_SIZE; r++) {
                        for (int c = 0; c < BOARD_SIZE; c++) {
                            Piece* piece = board.getPieceAt({ r, c });
                            if (!piece) continue;
                            sf::Sprite sprite;
                            int idx = (piece->getColor() == 1 ? 6 : 0);
                            switch (piece->getSymbol()) {
                            case 'P': idx += 0; break; case 'R': idx += 1; break; case 'N': idx += 2; break;
                            case 'B': idx += 3; break; case 'Q': idx += 4; break; case 'K': idx += 5; break;
                            }
                            sprite.setTexture(textures[idx]);
                            sprite.setPosition((float)(c * TILE_SIZE), (float)((7 - r) * TILE_SIZE));
                            sprite.setScale((float)TILE_SIZE / sprite.getLocalBounds().width, (float)TILE_SIZE / sprite.getLocalBounds().height);
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

                    // --- RYSOWANIE UI ---
                    window.draw(sidebarBg);

                    // Zegary
                    window.draw(labelBlack);
                    window.draw(blackTimerBox);
                    blackTimerText.setString(formatTime(timeBlack));
                    sf::FloatRect trB = blackTimerText.getLocalBounds();
                    blackTimerText.setOrigin(trB.left + trB.width / 2.0f, trB.top + trB.height / 2.0f);
                    blackTimerText.setPosition(blackTimerBox.getPosition().x + blackTimerBox.getSize().x / 2.0f, blackTimerBox.getPosition().y + blackTimerBox.getSize().y / 2.0f);
                    window.draw(blackTimerText);

                    window.draw(labelWhite);
                    window.draw(whiteTimerBox);
                    whiteTimerText.setString(formatTime(timeWhite));
                    sf::FloatRect trW = whiteTimerText.getLocalBounds();
                    whiteTimerText.setOrigin(trW.left + trW.width / 2.0f, trW.top + trW.height / 2.0f);
                    whiteTimerText.setPosition(whiteTimerBox.getPosition().x + whiteTimerBox.getSize().x / 2.0f, whiteTimerBox.getPosition().y + whiteTimerBox.getSize().y / 2.0f);
                    window.draw(whiteTimerText);

                    // Napisy informacyjne (Turn & Status)
                    if (!gameOver) {
                        turnText.setString(currentPlayer == 0 ? "Ruch: BIALE" : "Ruch: CZARNE");
                        turnText.setFillColor(sf::Color::White);
                    }
                    else {
                        turnText.setString("KONIEC GRY");
                        turnText.setFillColor(sf::Color::Yellow);
                    }

                    // Wyśrodkowanie tekstu "Ruch: ..."
                    centerText(turnText, (float)(BOARD_SIZE * TILE_SIZE / 2 - 60), (float)(BOARD_SIZE * TILE_SIZE), (float)SIDEBAR_WIDTH);
                    window.draw(turnText);

                    // Wyśrodkowanie tekstu Statusu (Szach / Mat / Czas)
                    centerText(statusText, (float)(BOARD_SIZE * TILE_SIZE / 2), (float)(BOARD_SIZE * TILE_SIZE), (float)SIDEBAR_WIDTH);
                    window.draw(statusText);

                    window.display();
                }

            }
        return 0;
    }