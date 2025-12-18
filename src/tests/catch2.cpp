#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include "../Board.h"
#include "../Piece.h"
#include "../Pawn.h"
#include "../Rook.h"
#include "../kNight.h"
#include "../Bishop.h"
#include "../Queen.h"
#include "../King.h"

// Pomocnicza funkcja do debugowania
void debugBoard(Board& board) {
    INFO("Stan planszy:");
    for (int r = 0; r < 8; r++) {
        std::string rowStr;
        for (int c = 0; c < 8; c++) {
            Piece* p = board.getPieceAt({ r, c });
            if (p == nullptr) rowStr += ". ";
            else rowStr += std::string(1, p->getSymbol()) + " ";
        }
        INFO(rowStr);
    }
}

TEST_CASE("Sample Test", "[sample]") {
    REQUIRE(1 + 1 == 2);
}

// --- TESTY WIE¯Y ---
TEST_CASE("Rook Tests", "[board][rook]") {
    Board board;
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    board.placePiece(rook);

    SECTION("Rook can move vertically") {
        REQUIRE(rook->canMove({ 3, 0 }, board) == true);
    }
    SECTION("Rook can move horizontally") {
        REQUIRE(rook->canMove({ 0, 5 }, board) == true);
    }
    SECTION("Rook cannot move when blocked") {
        Pawn* blocker = new Pawn(0, 'P', { 1, 0 });
        board.placePiece(blocker);

        REQUIRE(rook->canMove({ 3, 0 }, board) == false);

    }

}

// --- TESTY PIONA ---
TEST_CASE("Pawn Tests", "[board][pawn]") {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    board.placePiece(pawn);

    SECTION("Pawn can move forward") {
        REQUIRE(pawn->canMove({ 2, 1 }, board) == true);
    }
    SECTION("Pawn cannot move sideways") {
        REQUIRE(pawn->canMove({ 1, 2 }, board) == false);
    }
    SECTION("Pawn cannot move backward") {
        REQUIRE(pawn->canMove({ 0, 1 }, board) == false);
    }
    SECTION("Pawn cannot move two squares forward when blocked") {
        Pawn* blockingPawn = new Pawn(1, 'P', { 2, 1 });
        board.placePiece(blockingPawn);

        REQUIRE(pawn->canMove({ 3, 1 }, board) == false);

    }
    SECTION("Pawn cannot move when blocked directly") {
        Pawn* blockingPawn = new Pawn(1, 'P', { 2, 1 });
        board.placePiece(blockingPawn);

        REQUIRE(pawn->canMove({ 2, 1 }, board) == false);

    }

}

// --- TESTY SKOCZKA ---
TEST_CASE("Knight Tests", "[board][knight]") {
    Board board;
    Knight* knight = new Knight(0, 'N', { 4, 4 });
    board.placePiece(knight);

    SECTION("Knight can move in L shape") {
        REQUIRE(knight->canMove({ 6, 5 }, board) == true);
        REQUIRE(knight->canMove({ 5, 6 }, board) == true);
        REQUIRE(knight->canMove({ 2, 5 }, board) == true);
        REQUIRE(knight->canMove({ 2, 3 }, board) == true);
    }

}

// --- TESTY GOÑCA ---
TEST_CASE("Bishop Tests", "[board][bishop]") {
    Board board;
    Bishop* bishop = new Bishop(0, 'B', { 2, 0 });
    board.placePiece(bishop);

    SECTION("Bishop can move diagonally") {
        REQUIRE(bishop->canMove({ 5, 3 }, board) == true);
        REQUIRE(bishop->canMove({ 1, 1 }, board) == true);
    }
    SECTION("Bishop cannot move when blocked") {
        Pawn* blockingPawn = new Pawn(1, 'P', { 3, 1 });
        board.placePiece(blockingPawn);

        REQUIRE(bishop->canMove({ 5, 3 }, board) == false);

    }

}

// --- TESTY HETMANA ---
TEST_CASE("Queen Tests", "[board][queen]") {
    Board board;
    Queen* queen = new Queen(0, 'Q', { 3, 3 });
    board.placePiece(queen);

    SECTION("Queen can move vertically") {
        REQUIRE(queen->canMove({ 7, 3 }, board) == true);
    }
    SECTION("Queen can move horizontally") {
        REQUIRE(queen->canMove({ 3, 7 }, board) == true);
    }
    SECTION("Queen can move diagonally") {
        REQUIRE(queen->canMove({ 4, 4 }, board) == true);
    }
    SECTION("Queen cannot move when blocked") {
        Pawn* blockingPawn = new Pawn(1, 'P', { 5, 5 });
        board.placePiece(blockingPawn);

        REQUIRE(queen->canMove({ 6, 6 }, board) == false);

    }

}

// --- TESTY KRÓLA ---
TEST_CASE("King Tests", "[board][king]") {
    Board board;
    King* king = new King(0, 'K', { 4, 4 });
    board.placePiece(king);

    SECTION("King can move one square in any direction") {
        REQUIRE(king->canMove({ 5, 4 }, board) == true);
        REQUIRE(king->canMove({ 4, 5 }, board) == true);
        REQUIRE(king->canMove({ 3, 4 }, board) == true);
        REQUIRE(king->canMove({ 4, 3 }, board) == true);
        REQUIRE(king->canMove({ 5, 5 }, board) == true);
        REQUIRE(king->canMove({ 3, 3 }, board) == true);
    }
    SECTION("King cannot move more than one square") {
        REQUIRE(king->canMove({ 6, 4 }, board) == false);
        REQUIRE(king->canMove({ 4, 6 }, board) == false);
    }
    SECTION("King cannot move when blocked") {
        Pawn* blockingPawn = new Pawn(0, 'P', { 5, 4 });
        board.placePiece(blockingPawn);

        REQUIRE(king->canMove({ 5, 4 }, board) == false);

    }

}