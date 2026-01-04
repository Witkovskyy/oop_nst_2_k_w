#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
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
#include "engine/evalpos.cpp"
#include "engine/logger/logger.h"
#include <string>
#include <future>
#include "engine/tables/zobrist.h"

using namespace std;

// Global settings
int difficultyLevel = 2; // 1-Easy, 2-Medium, 3-Hard
int timeLimitMinutes = 10; // Default 10 minutes

const int TILE_SIZE = 80;
const int BOARD_SIZE = 8;
const int SIDEBAR_WIDTH = 260;

// Helper function to reset board pieces
void setupPieces(Board& board) {
    // White
    board.placePiece(new Rook(0, 'R', { 0, 0 })); board.placePiece(new Knight(0, 'N', { 0, 1 }));
    board.placePiece(new Bishop(0, 'B', { 0, 2 })); board.placePiece(new Queen(0, 'Q', { 0, 3 }));
    board.placePiece(new King(0, 'K', { 0, 4 })); board.placePiece(new Bishop(0, 'B', { 0, 5 }));
    board.placePiece(new Knight(0, 'N', { 0, 6 })); board.placePiece(new Rook(0, 'R', { 0, 7 }));
    for (int i = 0; i < BOARD_SIZE; i++) board.placePiece(new Pawn(0, 'P', { 1, i }));
    // Black
    board.placePiece(new Rook(1, 'R', { 7, 0 })); board.placePiece(new Knight(1, 'N', { 7, 1 }));
    board.placePiece(new Bishop(1, 'B', { 7, 2 })); board.placePiece(new Queen(1, 'Q', { 7, 3 }));
    board.placePiece(new King(1, 'K', { 7, 4 })); board.placePiece(new Bishop(1, 'B', { 7, 5 }));
    board.placePiece(new Knight(1, 'N', { 7, 6 })); board.placePiece(new Rook(1, 'R', { 7, 7 }));
    for (int i = 0; i < BOARD_SIZE; i++) board.placePiece(new Pawn(1, 'P', { 6, i }));
}

// Function centering text
void centerText(sf::Text& text, float x_center, float y_center) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(x_center, y_center);
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

    switch (difficultyLevel) {
    case 1: // Easy
        maxDepthAllowed = 2;
        timeLimitMs = 10;
        break;
    case 2: // Medium
        maxDepthAllowed = 4;
        timeLimitMs = 350;
        break;
    case 3: // Hard
        maxDepthAllowed = 64;
        timeLimitMs = 2000;
        break;
    }
    int aiSide = -1; // AI is playing black

    auto moves = legalMoves(boardCopy, to01(aiSide));
    std::string msg = "AI moves generated: " + std::to_string(moves.size());
    LOG(msg);

    if (moves.empty()) {
		// If no moves available, return an invalid move
        return Move{ {-1,-1}, {-1,-1}, nullptr, nullptr };
    }

    orderMoves(moves);
    sf::Clock clock;
    Move bestMoveOfAll = moves[0];
    bool timeUp = false;

    for (int currentDepth = 1; currentDepth <= maxDepthAllowed; ++currentDepth) {
        int alpha = -INF;
        int beta = INF;
        Move bestMoveThisDepth = moves[0];
        int bestScoreThisDepth = -1000000;

		// Primitive sorting: move best move from previous depth to front
        for (int i = 0; i < moves.size(); ++i) {
            if (moves[i].from.row == bestMoveOfAll.from.row &&
                moves[i].from.col == bestMoveOfAll.from.col &&
                moves[i].to.row == bestMoveOfAll.to.row &&
                moves[i].to.col == bestMoveOfAll.to.col) {
                std::swap(moves[0], moves[i]);
                break;
            }
        }

        for (auto& move : moves) {
            if (clock.getElapsedTime().asMilliseconds() > timeLimitMs) {
                timeUp = true;
                break;
            }
            Undo undo;
            applyMove(boardCopy, move, undo);
            int score = -negamax(boardCopy, currentDepth - 1, -beta, -alpha, -aiSide);
            undoMove(boardCopy, move, undo);

            if (score > bestScoreThisDepth) {
                bestScoreThisDepth = score;
                bestMoveThisDepth = move;
            }
            if (score > alpha) alpha = score;
        }
        if (timeUp) break;
        bestMoveOfAll = bestMoveThisDepth;

        // Easy mode blunder simulation
        if (difficultyLevel == 1 && moves.size() > 1) {
            if (rand() % 3 == 0) {
                int randomIdx = rand() % moves.size();
                bestMoveOfAll = moves[randomIdx];
            }
        }

        if (bestScoreThisDepth > 90000) break;
        if (currentDepth >= maxDepthAllowed) break;
    }
    return bestMoveOfAll;
}

int main() {
    std::future<Move> engineFuture;
    bool isEngineThinking = false;
    initZobrist();

    // Konfiguracja czasu (startowa)
    float initialTime = timeLimitMinutes * 60.0f;

    sf::RenderWindow window(sf::VideoMode(BOARD_SIZE * TILE_SIZE + SIDEBAR_WIDTH, BOARD_SIZE * TILE_SIZE), "CHESS - C++ Engine");

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        LOG("ERROR: Missing arial.ttf font file!");
    }

    // --- UI ELEMENTS SETUP ---
    sf::RectangleShape sidebarBg(sf::Vector2f((float)SIDEBAR_WIDTH, (float)(BOARD_SIZE * TILE_SIZE)));
    sidebarBg.setPosition((float)(BOARD_SIZE * TILE_SIZE), 0);
    sidebarBg.setFillColor(sf::Color(40, 40, 40));

    // Timers Boxes
    sf::Color timerBgColor(220, 220, 220);
    sf::Color timerTextColor = sf::Color::Black;

    sf::RectangleShape blackTimerBox(sf::Vector2f(SIDEBAR_WIDTH - 40.f, 60.f));
    blackTimerBox.setPosition((float)(BOARD_SIZE * TILE_SIZE + 20), 50.f);
    blackTimerBox.setFillColor(timerBgColor);

    sf::RectangleShape whiteTimerBox(sf::Vector2f(SIDEBAR_WIDTH - 40.f, 60.f));
    whiteTimerBox.setPosition((float)(BOARD_SIZE * TILE_SIZE + 20), (float)(BOARD_SIZE * TILE_SIZE - 110));
    whiteTimerBox.setFillColor(timerBgColor);

    // Text Objects
    sf::Text whiteTimerText, blackTimerText, turnText, statusText, labelWhite, labelBlack;
    whiteTimerText.setFont(font); whiteTimerText.setCharacterSize(40); whiteTimerText.setFillColor(timerTextColor);
    blackTimerText.setFont(font); blackTimerText.setCharacterSize(40); blackTimerText.setFillColor(timerTextColor);

    turnText.setFont(font); turnText.setCharacterSize(22); turnText.setFillColor(sf::Color::White);
    statusText.setFont(font); statusText.setCharacterSize(20); statusText.setFillColor(sf::Color(255, 80, 80)); statusText.setStyle(sf::Text::Bold);

    labelBlack.setFont(font); labelBlack.setString("GRACZ CZARNY"); labelBlack.setCharacterSize(16); labelBlack.setFillColor(sf::Color(180, 180, 180));
    labelBlack.setPosition((float)(BOARD_SIZE * TILE_SIZE + 25), 25.f);

    labelWhite.setFont(font); labelWhite.setString("GRACZ BIALY"); labelWhite.setCharacterSize(16); labelWhite.setFillColor(sf::Color(180, 180, 180));
    labelWhite.setPosition((float)(BOARD_SIZE * TILE_SIZE + 25), (float)(BOARD_SIZE * TILE_SIZE - 135));

    // --- BUTTONS SETUP ---
    float btnX = BOARD_SIZE * TILE_SIZE + 20;
    float btnY = 160; // Start Y position for controls

    // 1. Difficulty Section
    sf::Text diffLabel;
    diffLabel.setFont(font); diffLabel.setString("POZIOM TRUDNOSCI:");
    diffLabel.setCharacterSize(16); diffLabel.setFillColor(sf::Color::White);
    diffLabel.setPosition(btnX + 5, btnY);

    sf::RectangleShape btnEasy(sf::Vector2f(70, 40)); btnEasy.setPosition(btnX, btnY + 30);
    sf::RectangleShape btnMed(sf::Vector2f(70, 40));  btnMed.setPosition(btnX + 75, btnY + 30);
    sf::RectangleShape btnHard(sf::Vector2f(70, 40)); btnHard.setPosition(btnX + 150, btnY + 30);

    sf::Text txtEasy, txtMed, txtHard;
    txtEasy.setFont(font); txtEasy.setString("Latwy"); txtEasy.setCharacterSize(16); txtEasy.setFillColor(sf::Color::Black);
    txtMed.setFont(font); txtMed.setString("Sredni"); txtMed.setCharacterSize(16); txtMed.setFillColor(sf::Color::Black);
    txtHard.setFont(font); txtHard.setString("Trudny"); txtHard.setCharacterSize(16); txtHard.setFillColor(sf::Color::Black);

    centerText(txtEasy, btnX + 35, btnY + 50);
    centerText(txtMed, btnX + 75 + 35, btnY + 50);
    centerText(txtHard, btnX + 150 + 35, btnY + 50);

    // 2. Time Control Section
    float timeY = btnY + 90;
    sf::Text timeLabel;
    timeLabel.setFont(font); timeLabel.setString("CZAS GRY:");
    timeLabel.setCharacterSize(16); timeLabel.setFillColor(sf::Color::White);
    timeLabel.setPosition(btnX + 5, timeY);

    sf::RectangleShape btnTime1(sf::Vector2f(70, 40)); btnTime1.setPosition(btnX, timeY + 30);
    sf::RectangleShape btnTime5(sf::Vector2f(70, 40)); btnTime5.setPosition(btnX + 75, timeY + 30);
    sf::RectangleShape btnTime10(sf::Vector2f(70, 40)); btnTime10.setPosition(btnX + 150, timeY + 30);

    sf::Text txtT1, txtT5, txtT10;
    txtT1.setFont(font); txtT1.setString("1"); txtT1.setCharacterSize(16); txtT1.setFillColor(sf::Color::Black);
    txtT5.setFont(font); txtT5.setString("5"); txtT5.setCharacterSize(16); txtT5.setFillColor(sf::Color::Black);
    txtT10.setFont(font); txtT10.setString("10"); txtT10.setCharacterSize(16); txtT10.setFillColor(sf::Color::Black);

    centerText(txtT1, btnX + 35, timeY + 50);
    centerText(txtT5, btnX + 75 + 35, timeY + 50);
    centerText(txtT10, btnX + 150 + 35, timeY + 50);

    // 3. New Game Button
    float ngY = timeY + 90;
    sf::RectangleShape btnNewGame(sf::Vector2f(SIDEBAR_WIDTH - 40.f, 50.f));
    btnNewGame.setPosition(btnX, ngY);
    btnNewGame.setFillColor(sf::Color(100, 200, 100)); // Green

    sf::Text txtNewGame;
    txtNewGame.setFont(font); txtNewGame.setString("NOWA GRA"); txtNewGame.setCharacterSize(20); txtNewGame.setFillColor(sf::Color::Black);
    centerText(txtNewGame, btnX + (SIDEBAR_WIDTH - 40.f) / 2.0f, ngY + 25);


    // LOADING BOARD AND TEXTURES
    sf::Texture textures[12];
    textures[0].loadFromFile("pieces/White_Pawn.png"); textures[1].loadFromFile("pieces/White_Rook.png");
    textures[2].loadFromFile("pieces/White_Knight.png"); textures[3].loadFromFile("pieces/White_Bishop.png");
    textures[4].loadFromFile("pieces/White_Queen.png"); textures[5].loadFromFile("pieces/White_King.png");
    textures[6].loadFromFile("pieces/Black_Pawn.png"); textures[7].loadFromFile("pieces/Black_Rook.png");
    textures[8].loadFromFile("pieces/Black_Knight.png"); textures[9].loadFromFile("pieces/Black_Bishop.png");
    textures[10].loadFromFile("pieces/Black_Queen.png"); textures[11].loadFromFile("pieces/Black_King.png");

    // Init Logic
    Board* board = new Board(); // Pointer to handle reset easier
    setupPieces(*board);
    board->computeZobristHash();
    board->positionHistory.push_back(board->zobristKey);

    int currentPlayer = 0;
    Position selected = { -1, -1 };
    Piece* selectedPiece = nullptr;
    bool gameOver = false;
    vector<Position> validMoves;

    float timeWhite = initialTime;
    float timeBlack = initialTime;
    sf::Clock dtClock;

    LOG("Game started.");

    while (window.isOpen()) {
        float dt = dtClock.restart().asSeconds();

        // --- UPDATE BUTTON COLORS ---
        // Difficulty
        btnEasy.setFillColor(difficultyLevel == 1 ? sf::Color::Yellow : sf::Color(150, 150, 150));
        btnMed.setFillColor(difficultyLevel == 2 ? sf::Color::Yellow : sf::Color(150, 150, 150));
        btnHard.setFillColor(difficultyLevel == 3 ? sf::Color::Yellow : sf::Color(150, 150, 150));

        // Time
        btnTime1.setFillColor(timeLimitMinutes == 1 ? sf::Color::Cyan : sf::Color(150, 150, 150));
        btnTime5.setFillColor(timeLimitMinutes == 5 ? sf::Color::Cyan : sf::Color(150, 150, 150));
        btnTime10.setFillColor(timeLimitMinutes == 10 ? sf::Color::Cyan : sf::Color(150, 150, 150));


        if (!gameOver) {
            if (currentPlayer == 0) timeWhite -= dt;
            else timeBlack -= dt;

            if (timeWhite <= 0) { gameOver = true; statusText.setString("KONIEC CZASU!\nWygrywaja CZARNE"); timeWhite = 0; }
            if (timeBlack <= 0) { gameOver = true; statusText.setString("KONIEC CZASU!\nWygrywaja BIALE"); timeBlack = 0; }
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mx = event.mouseButton.x;
                int my = event.mouseButton.y;

                // --- UI INTERACTION ---
                bool resetNeeded = false;

                // 1. Difficulty Selection
                if (btnEasy.getGlobalBounds().contains((float)mx, (float)my)) difficultyLevel = 1;
                else if (btnMed.getGlobalBounds().contains((float)mx, (float)my)) difficultyLevel = 2;
                else if (btnHard.getGlobalBounds().contains((float)mx, (float)my)) difficultyLevel = 3;

                // 2. Time Selection (Triggers Reset)
                else if (btnTime1.getGlobalBounds().contains((float)mx, (float)my)) {
                    timeLimitMinutes = 1; resetNeeded = true;
                }
                else if (btnTime5.getGlobalBounds().contains((float)mx, (float)my)) {
                    timeLimitMinutes = 5; resetNeeded = true;
                }
                else if (btnTime10.getGlobalBounds().contains((float)mx, (float)my)) {
                    timeLimitMinutes = 10; resetNeeded = true;
                }

                // 3. New Game Button (Triggers Reset)
                else if (btnNewGame.getGlobalBounds().contains((float)mx, (float)my)) {
                    resetNeeded = true;
                }

                // --- HANDLE RESET LOGIC ---
                if (resetNeeded && !isEngineThinking) {
                    LOG("Resetting Game...");
                    delete board;
                    board = new Board();
                    setupPieces(*board);
                    board->computeZobristHash();
                    board->positionHistory.clear();
                    board->positionHistory.push_back(board->zobristKey);

                    currentPlayer = 0;
                    selected = { -1, -1 };
                    selectedPiece = nullptr;
                    gameOver = false;
                    validMoves.clear();

                    // Reset time
                    initialTime = timeLimitMinutes * 60.0f;
                    timeWhite = initialTime;
                    timeBlack = initialTime;

                    statusText.setString("");
                    continue; // Skip rest of loop
                }


                // --- BOARD INTERACTION (Only if not clicking sidebar) ---
                if (!isEngineThinking && currentPlayer == 0 && !gameOver && mx < BOARD_SIZE * TILE_SIZE) {

                    int col = mx / TILE_SIZE;
                    int row = 7 - (my / TILE_SIZE);
                    Position clickedPos = { row, col };

                    if (selectedPiece == nullptr) {
                        Piece* clickedPiece = board->getPieceAt(clickedPos);
                        if (clickedPiece && clickedPiece->getColor() == currentPlayer) {
                            selectedPiece = clickedPiece;
                            selected = clickedPos;
                            validMoves.clear();
                            for (int r = 0; r < 8; r++) {
                                for (int c = 0; c < 8; c++) {
                                    if (board->isMoveSafe(selected, { r, c })) {
                                        validMoves.push_back({ r, c });
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (clickedPos.row == selected.row && clickedPos.col == selected.col) {
                            selectedPiece = nullptr;
                            selected = { -1, -1 };
                            validMoves.clear();
                        }
                        else if (board->getPieceAt(clickedPos) && board->getPieceAt(clickedPos)->getColor() == currentPlayer) {
                            selectedPiece = board->getPieceAt(clickedPos);
                            selected = clickedPos;
                            // Recalculate moves for new selection
                            validMoves.clear();
                            for (int r = 0; r < 8; r++) {
                                for (int c = 0; c < 8; c++) {
                                    if (board->isMoveSafe(selected, { r, c })) {
                                        validMoves.push_back({ r, c });
                                    }
                                }
                            }
                        }
                        else {
                            bool isLegal = false;
                            for (Position p : validMoves) {
                                if (p.row == clickedPos.row && p.col == clickedPos.col) {
                                    isLegal = true; break;
                                }
                            }

                            if (isLegal) {
                                Piece* captured = board->getPieceAt(clickedPos);
                                board->movePiece(selected, clickedPos, selectedPiece);
                                if (captured) delete captured;

                                if (selectedPiece->getSymbol() == 'P' && (clickedPos.row == 0 || clickedPos.row == 7)) {
                                    board->promotePawn(*board, clickedPos, 'Q', currentPlayer);
                                }

                                currentPlayer = 1 - currentPlayer;

                                if (board->isKingInCheck(currentPlayer)) {
                                    if (board->isCheckMate(currentPlayer)) {
                                        statusText.setString("MAT!\nWygrywa gracz:\n" + string(currentPlayer == 0 ? "CZARNY" : "BIALY"));
                                        gameOver = true;
                                    }
                                    else statusText.setString("SZACH!");
                                }
                                else statusText.setString("");

                                selectedPiece = nullptr;
                                selected = { -1, -1 };
                                validMoves.clear();
                            }
                        }
                    }

                }
            }
        }

        window.clear();

        // Draw Board
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                sf::RectangleShape square(sf::Vector2f((float)TILE_SIZE, (float)TILE_SIZE));
                square.setPosition((float)(c * TILE_SIZE), (float)((7 - r) * TILE_SIZE));
                if ((r + c) % 2 == 0) square.setFillColor(sf::Color(240, 217, 181));
                else square.setFillColor(sf::Color(181, 136, 99));
                if (selectedPiece && r == selected.row && c == selected.col) square.setFillColor(sf::Color(118, 150, 86));

                Piece* p = board->getPieceAt({ r,c });
                if (p && p->getSymbol() == 'K' && p->getColor() == currentPlayer && board->isKingInCheck(currentPlayer))
                    square.setFillColor(sf::Color(200, 50, 50));
                window.draw(square);
            }
        }

        // Draw Moves hints
        for (Position pos : validMoves) {
            float radius = TILE_SIZE / 6.0f;
            sf::CircleShape dot(radius);
            dot.setFillColor(sf::Color(100, 100, 100, 128));
            dot.setOrigin(radius, radius);
            dot.setPosition((float)(pos.col * TILE_SIZE) + (TILE_SIZE / 2.0f), (float)((7 - pos.row) * TILE_SIZE) + (TILE_SIZE / 2.0f));
            if (!board->isEmpty(pos)) {
                dot.setRadius(TILE_SIZE / 2.2f); dot.setOrigin(dot.getRadius(), dot.getRadius());
                dot.setFillColor(sf::Color::Transparent); dot.setOutlineThickness(5);
                dot.setOutlineColor(sf::Color(100, 100, 100, 128));
            }
            window.draw(dot);
        }

        // Draw Pieces
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                Piece* piece = board->getPieceAt({ r, c });
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

        // --- DRAW UI ---
        window.draw(sidebarBg);

        window.draw(labelBlack);
        window.draw(blackTimerBox);
        blackTimerText.setString(formatTime(timeBlack));
        centerText(blackTimerText, blackTimerBox.getPosition().x + blackTimerBox.getSize().x / 2.0f, blackTimerBox.getPosition().y + blackTimerBox.getSize().y / 2.0f);
        window.draw(blackTimerText);

        window.draw(labelWhite);
        window.draw(whiteTimerBox);
        whiteTimerText.setString(formatTime(timeWhite));
        centerText(whiteTimerText, whiteTimerBox.getPosition().x + whiteTimerBox.getSize().x / 2.0f, whiteTimerBox.getPosition().y + whiteTimerBox.getSize().y / 2.0f);
        window.draw(whiteTimerText);

        if (!gameOver) {
            turnText.setString(currentPlayer == 0 ? "Ruch: BIALE" : "Ruch: CZARNE");
            turnText.setFillColor(sf::Color::White);
        }
        else {
            turnText.setString("KONIEC GRY");
            turnText.setFillColor(sf::Color::Yellow);
        }
        centerText(turnText, (float)(BOARD_SIZE * TILE_SIZE + SIDEBAR_WIDTH / 2.0f), 20.f);
        window.draw(turnText);

        centerText(statusText, (float)(BOARD_SIZE * TILE_SIZE + SIDEBAR_WIDTH / 2.0f), (float)(BOARD_SIZE * TILE_SIZE - 200));
        window.draw(statusText);

        // Draw Buttons
        // Difficulty
        window.draw(diffLabel);
        window.draw(btnEasy); window.draw(txtEasy);
        window.draw(btnMed);  window.draw(txtMed);
        window.draw(btnHard); window.draw(txtHard);

        // Time
        window.draw(timeLabel);
        window.draw(btnTime1); window.draw(txtT1);
        window.draw(btnTime5); window.draw(txtT5);
        window.draw(btnTime10); window.draw(txtT10);

        // New Game
        window.draw(btnNewGame); window.draw(txtNewGame);

        window.display();

        // AI Logic Handling
        if (currentPlayer == 1)
        {
            if (!isEngineThinking) {
                isEngineThinking = true;
                engineFuture = std::async(std::launch::async, runEngineAsync, *board, difficultyLevel);
            }
            if (engineFuture.valid() && engineFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                Move bestMoveOfAll = engineFuture.get();
                if (bestMoveOfAll.pieceMoved == nullptr) {
                    if (isInCheck(*board, 1)) statusText.setString("SZACH MAT!\nWygrywaja BIALE");
                    else statusText.setString("PAT!\nRemis");
                    gameOver = true;
                    isEngineThinking = false;
                }
                else {
                    Position from = bestMoveOfAll.from;
                    Position to = bestMoveOfAll.to;
                    Piece* realPiece = board->getPieceAt(from);
                    Piece* captured = board->getPieceAt(to);
                    if (realPiece) {
                        board->movePiece(from, to, realPiece);
                        if (captured) delete captured;
                        if (realPiece->getSymbol() == 'P' && (to.row == 0 || to.row == 7)) {
                            board->promotePawn(*board, to, 'Q', currentPlayer);
                        }
                        board->computeZobristHash();
                        board->positionHistory.push_back(board->zobristKey);
                        currentPlayer = 0;
                    }
                    isEngineThinking = false;

                    if (board->isKingInCheck(currentPlayer)) {
                        if (board->isCheckMate(currentPlayer)) {
                            statusText.setString("MAT!\nWygrywa gracz:\n" + string(currentPlayer == 0 ? "CZARNY" : "BIALY"));
                            gameOver = true;
                        }
                        else statusText.setString("SZACH!");
                    }
                }
            }
        }
    }

    delete board;
    return 0;
}