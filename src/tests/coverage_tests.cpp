#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../Board.h"
#include "../Piece.h"
#include "../Pawn.h"
#include "../Rook.h"
#include "../kNight.h"
#include "../Bishop.h"
#include "../Queen.h"
#include "../King.h"
#include "../engine/tables/TT.h"
#include "../engine/tables/zobrist.h"
#include "../engine/engine.h"
#include "../engine/val.h"
#include "../engine/logger/logger.h"
#include <thread>
#include <chrono>

// --- Helpers ---
void clearBoard(Board& board) {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (board.squares[r][c]) {
                delete board.squares[r][c];
                board.squares[r][c] = nullptr;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// 1. LOGGER TESTS
// -----------------------------------------------------------------------------
TEST_CASE("AsyncLogger coverage", "[Logger]") {
    LOG("Test");
    { AsyncLogger logger; logger.log("Msg"); }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(true);
}

// -----------------------------------------------------------------------------
// 2. TT & ZOBRIST TESTS
// -----------------------------------------------------------------------------
TEST_CASE("Transposition Table Coverage", "[TT]") {
    TT.clear();
    SECTION("Store and Probe Logic") {
        Move m; m.from = { 0,0 }; m.to = { 1,1 };
        TT.store(111, 100, 5, TT_EXACT, m);
        int score; Move outM;
        // Hit
        REQUIRE(TT.probe(111, 5, -1000, 1000, score, outM));
        REQUIRE(score == 100);
        // Miss (key)
        REQUIRE(!TT.probe(222, 5, -1000, 1000, score, outM));
        // Miss (depth)
        REQUIRE(!TT.probe(111, 6, -1000, 1000, score, outM));
    }
}

TEST_CASE("Zobrist Init", "[Zobrist]") {
    initZobrist();
    REQUIRE(sideKey != 0);
}

// -----------------------------------------------------------------------------
// 3. PIECE TESTS
// -----------------------------------------------------------------------------
TEST_CASE("Piece Coverage", "[Piece]") {
    Board b;
    SECTION("Piece Getters/Setters") {
        Piece p(0, 'P', { 0,0 });
        p.setColor(1); p.setSymbol('R'); p.setPosition({ 3,3 });
        REQUIRE(p.getColor() == 1);
        REQUIRE(p.getSymbol() == 'R');
    }
    SECTION("Pawn Coverage") {
        Pawn* wp = new Pawn(0, 'P', { 1, 1 }); b.placePiece(wp);
        REQUIRE(wp->canMove({ 3, 1 }, b));
        REQUIRE(wp->canMove({ 2, 1 }, b));
        REQUIRE(!wp->canMove({ 2, 2 }, b));
        b.placePiece(new Pawn(1, 'P', { 2, 2 }));
        REQUIRE(wp->canMove({ 2, 2 }, b));
    }
}

// -----------------------------------------------------------------------------
// 4. BOARD TESTS
// -----------------------------------------------------------------------------
TEST_CASE("Board Internals Coverage", "[Board]") {
    Board b1;
    b1.placePiece(new Rook(0, 'R', { 0,0 }));
    SECTION("Copy Constructor") {
        Board b2 = b1;
        REQUIRE(b2.getPieceAt({ 0,0 }) != nullptr);
    }
    SECTION("isSquareAttacked - Full Coverage") {
        Board b;
        Position center = { 4,4 };
        b.placePiece(new Rook(1, 'R', { 4,0 }));
        REQUIRE(b.isSquareAttacked(center, 1));
    }
    SECTION("isMoveSafe - Pin logic") {
        Board b;
        b.placePiece(new King(0, 'K', { 0,0 }));
        b.placePiece(new Rook(0, 'R', { 0,1 }));
        b.placePiece(new Rook(1, 'R', { 0,7 }));
        // Rook is pinned
        REQUIRE(!b.isMoveSafe({ 0,1 }, { 1,1 }));
        // Moving along the pin is allowed
        REQUIRE(b.isMoveSafe({ 0,1 }, { 0,2 }));
    }
}

// -----------------------------------------------------------------------------
// 5. ENGINE LOGIC TESTS
// -----------------------------------------------------------------------------
TEST_CASE("Engine Logic Coverage", "[Engine]") {
    Board b;
    b.placePiece(new King(0, 'K', { 0,0 }));
    b.placePiece(new King(1, 'K', { 7,7 }));

    SECTION("Eval Function") {
        REQUIRE(eval(b, 1) == 0);
        b.placePiece(new Pawn(0, 'P', { 1,1 }));
        REQUIRE(eval(b, 1) > 0);

        int scoreEdge = eval(b, 1);
        delete b.squares[1][1]; b.squares[1][1] = nullptr;
        // Place pawn in center manually
        b.placePiece(new Pawn(0, 'P', { 1,4 }));
        int scoreCenter = eval(b, 1);
        // PST should make a difference
        CHECK(scoreCenter != scoreEdge);
    }

    SECTION("SEE (Static Exchange Evaluation)") {
        b.placePiece(new Pawn(0, 'P', { 4,4 }));
        b.placePiece(new Pawn(1, 'P', { 5,5 }));
        REQUIRE(see(b, { 4,4 }, 1) >= 0);
        REQUIRE(see(b, { 0,0 }, 1) == 0);
    }

    SECTION("Checkmate and Stalemate detection in Negamax") {
        clearBoard(b);

        // --- SCENARIO: ULTIMATE CHECKMATE ---
        // 1. White King in corner (0,0) - victim
        b.placePiece(new King(0, 'K', { 0,0 }));

        // 2. Black King in opposite corner (MUST BE THERE, otherwise gameOver() = true)
        b.placePiece(new King(1, 'K', { 7,7 }));

        // 3. Black Queens surrounding White King (Contact Checkmate)
        b.placePiece(new Queen(1, 'Q', { 0,1 })); // Attacks and cuts off escape
        b.placePiece(new Queen(1, 'Q', { 1,0 })); // Attacks and cuts off escape
        b.placePiece(new Queen(1, 'Q', { 1,1 })); // Protects others

        // Verification: Does engine see check?
        REQUIRE(isInCheck(b, 0) == true);

        // Run negamax for White (Sign=1 -> ID=0)
        // Expecting result indicating loss (very low negative value)
        int score = negamax(b, 1, -100000, 100000, 1);

        REQUIRE(score < -10000);
    }
}