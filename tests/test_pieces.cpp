/**
 * @brief Test helper: make empty board.
 *
 * @details Used by the unit/integration test suite.
 * @return Result of the operation.
 */


#include <gtest/gtest.h>
#include "../src/Board.h"
#include "../src/Pawn.h"
#include "../src/Rook.h"
#include "../src/Piece.h"

// Make empty board
/**
 * @brief Test helper: make empty board.
 *
 * @details Used by the unit/integration test suite.
 * @return Result of the operation.
 */
std::unique_ptr<Board> makeEmptyBoard() {
    return std::make_unique<Board>();
}

TEST(BoardTest, PlacePieceAndGetPieceAt) {
    Board board;
    /**
 * @brief Test helper: capture stdout.
 *
 * @details Used by the unit/integration test suite.
 * @return Result of the operation.
 */
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    board.placePiece(pawn);
    EXPECT_EQ(board.getPieceAt({ 1, 1 }), pawn);
}

TEST(BoardTest, IsEmpty) {
    Board board;
    /**
 * @brief Test helper: capture stdout.
 *
 * @details Used by the unit/integration test suite.
 * @return Result of the operation.
 */
    EXPECT_TRUE(board.isEmpty({ 2, 2 }));
    Pawn* pawn = new Pawn(0, 'P', { 2, 2 });
    board.placePiece(pawn);
    EXPECT_FALSE(board.isEmpty({ 2, 2 }));
}

/**
 * @brief Test helper: capture stdout.
 *
 * @details Used by the unit/integration test suite.
 * @return Result of the operation.
 */
TEST(BoardTest, ValidateMove) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    board.placePiece(pawn);
    // Valid move
    EXPECT_TRUE(board.validateMove({ 1, 1 }, { 2, 1 }, pawn));
    // Invalid moves
    /**
 * @brief Test helper: capture stdout.
 *
 * @details Used by the unit/integration test suite.
 * @return Result of the operation.
 */
    EXPECT_FALSE(board.validateMove({ 1, 1 }, { 8, 1 }, pawn));
    EXPECT_FALSE(board.validateMove({ 1, 1 }, { 1, 8 }, pawn));
    EXPECT_FALSE(board.validateMove({ 1, 1 }, { -1, 1 }, pawn));
}

TEST(BoardTest, MovePiece_ValidAndInvalid) {
    Board board;
    /**
 * @brief Test helper: capture stdout.
 *
 * @details Used by the unit/integration test suite.
 * @return Result of the operation.
 */
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    board.placePiece(pawn);
    // Valid move
    EXPECT_TRUE(board.movePiece({ 1, 1 }, { 2, 1 }, pawn));
    // Invalid moves
    EXPECT_FALSE(board.movePiece({ 2, 1 }, { 4, 1 }, pawn));
}

/**
 * @brief Test helper: capture stdout.
 *
 * @details Used by the unit/integration test suite.
 * @return Result of the operation.
 */
TEST(BoardTest, DisplayBoard) {
    Board board;
    testing::internal::CaptureStdout();
    board.DisplayBoard();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("a b c d e f g h"), std::string::npos);
}

/**
 * @brief Perform `TEST`.
 *
 * @details Documentation for `TEST`.
 * @param BoardTest Parameter.
 * @param GetPieceAt_OutOfBounds Parameter.
 */
TEST(BoardTest, GetPieceAt_OutOfBounds) {
    Board board;
    EXPECT_EQ(board.getPieceAt({ -1, 0 }), nullptr);
    EXPECT_EQ(board.getPieceAt({ 0, -1 }), nullptr);
    EXPECT_EQ(board.getPieceAt({ 8, 0 }), nullptr);
    EXPECT_EQ(board.getPieceAt({ 0, 8 }), nullptr);
}

//Pawn

/**
 * @brief Perform `TEST`.
 *
 * @details Documentation for `TEST`.
 * @param PawnTest Parameter.
 * @param CanMove_ForwardOne Parameter.
 */
TEST(PawnTest, CanMove_ForwardOne) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    board.placePiece(pawn);
    EXPECT_TRUE(pawn->canMove({ 2, 1 }, &board));
}

/**
 * @brief Perform `TEST`.
 *
 * @details Documentation for `TEST`.
 * @param PawnTest Parameter.
 * @param CanMove_ForwardTwoFromStart Parameter.
 */
TEST(PawnTest, CanMove_ForwardTwoFromStart) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    board.placePiece(pawn);
    EXPECT_TRUE(pawn->canMove({ 3, 1 }, &board));
}

/**
 * @brief Perform `TEST`.
 *
 * @details Documentation for `TEST`.
 * @param PawnTest Parameter.
 * @param CanMove_ForwardTwoNotFromStart Parameter.
 */
TEST(PawnTest, CanMove_ForwardTwoNotFromStart) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 2, 1 });
    board.placePiece(pawn);
    EXPECT_FALSE(pawn->canMove({ 4, 1 }, &board));
}

/**
 * @brief Perform `TEST`.
 *
 * @details Documentation for `TEST`.
 * @param PawnTest Parameter.
 * @param CanMove_Capture Parameter.
 */
TEST(PawnTest, CanMove_Capture) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    Pawn* enemy = new Pawn(1, 'P', { 2, 2 });
    board.placePiece(pawn);
    board.placePiece(enemy);
    EXPECT_TRUE(pawn->canMove({ 2, 2 }, &board));
}

/**
 * @brief Perform `TEST`.
 *
 * @details Documentation for `TEST`.
 * @param PawnTest Parameter.
 * @param CanMove_BlockForward Parameter.
 */
TEST(PawnTest, CanMove_BlockForward) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    Pawn* block = new Pawn(1, 'P', { 2, 1 });
    board.placePiece(pawn);
    board.placePiece(block);
    EXPECT_FALSE(pawn->canMove({ 2, 1 }, &board));
}

/**
 * @brief Perform `TEST`.
 *
 * @details Documentation for `TEST`.
 * @param PawnTest Parameter.
 * @param CanMove_OutOfBoard Parameter.
 */
TEST(PawnTest, CanMove_OutOfBoard) {
    Board board;
    Pawn* pawn = new Pawn(0, 'P', { 1, 1 });
    board.placePiece(pawn);
    EXPECT_FALSE(pawn->canMove({ 8, 1 }, &board));
    EXPECT_FALSE(pawn->canMove({ 1, 8 }, &board));
    EXPECT_FALSE(pawn->canMove({ -1, 1 }, &board));
    /**
     * @brief Perform `EXPECT_FALSE`.
     *
     * @details Documentation for `EXPECT_FALSE`.
     * @param board Board state to operate on.
     */
    EXPECT_FALSE(pawn->canMove({ 1, -1 }, &board));
}

//Rook

TEST(RookTest, CanMove_Vertical) {
    Board board;
    /**
     * @brief Perform `Rook`.
     *
     * @details Documentation for `Rook`.
     * @param R Parameter.
     * @return Result of the operation.
     */
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    board.placePiece(rook);
    EXPECT_TRUE(rook->canMove({ 3, 0 }, &board));
}

TEST(RookTest, CanMove_Horizontal) {
    Board board;
    /**
     * @brief Perform `Rook`.
     *
     * @details Documentation for `Rook`.
     * @param R Parameter.
     * @return Result of the operation.
     */
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    board.placePiece(rook);
    EXPECT_TRUE(rook->canMove({ 0, 5 }, &board));
}

TEST(RookTest, CanMove_Blocked) {
    Board board;
    /**
     * @brief Perform `Rook`.
     *
     * @details Documentation for `Rook`.
     * @param R Parameter.
     * @return Result of the operation.
     */
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    Pawn* pawn = new Pawn(0, 'P', { 1, 0 });
    board.placePiece(rook);
    board.placePiece(pawn);
    EXPECT_FALSE(rook->canMove({ 3, 0 }, &board));
}

/**
 * @brief Perform `TEST`.
 *
 * @details Documentation for `TEST`.
 * @param RookTest Parameter.
 * @param CanMove_CaptureEnemy Parameter.
 */
TEST(RookTest, CanMove_CaptureEnemy) {
    Board board;
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    Pawn* enemy = new Pawn(1, 'P', { 0, 3 });
    board.placePiece(rook);
    board.placePiece(enemy);
    EXPECT_TRUE(rook->canMove({ 0, 3 }, &board));
}

/**
 * @brief Perform `TEST`.
 *
 * @details Documentation for `TEST`.
 * @param RookTest Parameter.
 * @param CanMove_CannotCaptureOwn Parameter.
 */
TEST(RookTest, CanMove_CannotCaptureOwn) {
    Board board;
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    Pawn* own = new Pawn(0, 'P', { 0, 3 });
    board.placePiece(rook);
    board.placePiece(own);
    EXPECT_FALSE(rook->canMove({ 0, 3 }, &board));
}

/**
 * @brief Perform `TEST`.
 *
 * @details Documentation for `TEST`.
 * @param RookTest Parameter.
 * @param CanMove_OutOfBoard Parameter.
 */
TEST(RookTest, CanMove_OutOfBoard) {
    Board board;
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    board.placePiece(rook);
    EXPECT_FALSE(rook->canMove({ 8, 0 }, &board));
    EXPECT_FALSE(rook->canMove({ 0, 8 }, &board));
    EXPECT_FALSE(rook->canMove({ -1, 0 }, &board));
    /**
     * @brief Perform `EXPECT_FALSE`.
     *
     * @details Documentation for `EXPECT_FALSE`.
     * @param board Board state to operate on.
     */
    EXPECT_FALSE(rook->canMove({ 0, -1 }, &board));
}

TEST(RookTest, CanMove_NoMove) {
    Board board;
    Rook* rook = new Rook(0, 'R', { 0, 0 });
    /**
     * @brief Place a piece on the board at its current position.
     *
     * @details Documentation for `placePiece`.
     * @param rook Parameter.
     * @return Result of the operation.
     */
    board.placePiece(rook);
    EXPECT_FALSE(rook->canMove({ 0, 0 }, &board));
}
