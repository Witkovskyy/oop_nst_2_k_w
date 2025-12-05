#include <gtest/gtest.h>
#include "../src/Board.h"
#include "../src/Pawn.h"
#include "../src/Rook.h"
#include "../src/Piece.h"

// Make empty board
std::unique_ptr<Board> makeEmptyBoard() {
    return std::make_unique<Board>();
}

TEST(BoardTest, PlacePieceAndGetPieceAt) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    board.placePiece(pawn);
    EXPECT_EQ(board.getPieceAt({ 1, 1 }), pawn);
}

TEST(BoardTest, IsEmpty) {
    Board board;
    EXPECT_TRUE(board.isEmpty({ 2, 2 }));
    Pawn* pawn = new Pawn(0, 'P', { 2, 2 });
    board.placePiece(pawn);
    EXPECT_FALSE(board.isEmpty({ 2, 2 }));
}

TEST(BoardTest, ValidateMove) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    board.placePiece(pawn);
    // Valid move
    EXPECT_TRUE(board.validateMove({ 1, 1 }, { 2, 1 }, pawn));
    // Invalid moves
    EXPECT_FALSE(board.validateMove({ 1, 1 }, { 8, 1 }, pawn));
    EXPECT_FALSE(board.validateMove({ 1, 1 }, { 1, 8 }, pawn));
    EXPECT_FALSE(board.validateMove({ 1, 1 }, { -1, 1 }, pawn));
}

TEST(BoardTest, MovePiece_ValidAndInvalid) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    board.placePiece(pawn);
    // Valid move
    EXPECT_TRUE(board.movePiece({ 1, 1 }, { 2, 1 }, pawn));
    // Invalid moves
    EXPECT_FALSE(board.movePiece({ 2, 1 }, { 4, 1 }, pawn));
}

TEST(BoardTest, DisplayBoard) {
    Board board;
    testing::internal::CaptureStdout();
    board.DisplayBoard();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("a b c d e f g h"), std::string::npos);
}

TEST(BoardTest, GetPieceAt_OutOfBounds) {
    Board board;
    EXPECT_EQ(board.getPieceAt({ -1, 0 }), nullptr);
    EXPECT_EQ(board.getPieceAt({ 0, -1 }), nullptr);
    EXPECT_EQ(board.getPieceAt({ 8, 0 }), nullptr);
    EXPECT_EQ(board.getPieceAt({ 0, 8 }), nullptr);
}

//Pawn

TEST(PawnTest, CanMove_ForwardOne) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    board.placePiece(pawn);
    EXPECT_TRUE(pawn->canMove({ 2, 1 }, &board));
}

TEST(PawnTest, CanMove_ForwardTwoFromStart) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    board.placePiece(pawn);
    EXPECT_TRUE(pawn->canMove({ 3, 1 }, &board));
}

TEST(PawnTest, CanMove_ForwardTwoNotFromStart) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 2, 1 });
    board.placePiece(pawn);
    EXPECT_FALSE(pawn->canMove({ 4, 1 }, &board));
}

TEST(PawnTest, CanMove_Capture) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    Pawn* enemy = new Pawn(1, 'P', { 2, 2 });
    board.placePiece(pawn);
    board.placePiece(enemy);
    EXPECT_TRUE(pawn->canMove({ 2, 2 }, &board));
}

TEST(PawnTest, CanMove_BlockForward) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    Pawn* block = new Pawn(1, 'P', { 2, 1 });
    board.placePiece(pawn);
    board.placePiece(block);
    EXPECT_FALSE(pawn->canMove({ 2, 1 }, &board));
}

TEST(PawnTest, CanMove_OutOfBoard) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    board.placePiece(pawn);
    EXPECT_FALSE(pawn->canMove({ 8, 1 }, &board));
    EXPECT_FALSE(pawn->canMove({ 1, 8 }, &board));
    EXPECT_FALSE(pawn->canMove({ -1, 1 }, &board));
    EXPECT_FALSE(pawn->canMove({ 1, -1 }, &board));
}

//Rook

TEST(RookTest, CanMove_Vertical) {
    Board board;
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    board.placePiece(rook);
    EXPECT_TRUE(rook->canMove({ 3, 0 }, &board));
}

TEST(RookTest, CanMove_Horizontal) {
    Board board;
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    board.placePiece(rook);
    EXPECT_TRUE(rook->canMove({ 0, 5 }, &board));
}

TEST(RookTest, CanMove_Blocked) {
    Board board;
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    Pawn* pawn = new Pawn(0, 'P', { 1, 0 });
    board.placePiece(rook);
    board.placePiece(pawn);
    EXPECT_FALSE(rook->canMove({ 3, 0 }, &board));
}

TEST(RookTest, CanMove_CaptureEnemy) {
    Board board;
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    Pawn* enemy = new Pawn(1, 'P', { 0, 3 });
    board.placePiece(rook);
    board.placePiece(enemy);
    EXPECT_TRUE(rook->canMove({ 0, 3 }, &board));
}

TEST(RookTest, CanMove_CannotCaptureOwn) {
    Board board;
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    Pawn* own = new Pawn(0, 'P', { 0, 3 });
    board.placePiece(rook);
    board.placePiece(own);
    EXPECT_FALSE(rook->canMove({ 0, 3 }, &board));
}

TEST(RookTest, CanMove_OutOfBoard) {
    Board board;
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    board.placePiece(rook);
    EXPECT_FALSE(rook->canMove({ 8, 0 }, &board));
    EXPECT_FALSE(rook->canMove({ 0, 8 }, &board));
    EXPECT_FALSE(rook->canMove({ -1, 0 }, &board));
    EXPECT_FALSE(rook->canMove({ 0, -1 }, &board));
}

TEST(RookTest, CanMove_NoMove) {
    Board board;
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    board.placePiece(rook);
    EXPECT_FALSE(rook->canMove({ 0, 0 }, &board));
}
