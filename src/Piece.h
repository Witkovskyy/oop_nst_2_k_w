/**
 * @file Piece.h
 * @brief File declaration for Piece base class.
 * @version 0.1
 * @date 2026-01-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once
#include <iostream>
using namespace std;
/**
 * @brief Structure to represent a position on the chess board.
 * 
 */
class Board;

/**
 * @brief Structure to represent a position on the chess board.
 * 
 */
struct Position {
    int row;
    int col;
};

class Board;

/**
 * @brief Base class for all chess pieces.
 * 
 */
class Piece
{
    public:
    /**
     * @brief Color of the piece.
     * 
     */
        int color;
    /**
     * @brief Symbol of the piece.
     * 
     */
        char symbol;
    /**
     * @brief Current position of the piece.
     * 
     */
        Position current_position;
    /**
     * @brief Value of the piece.
     * 
     */
        int value;
    public:
        /**
         * @brief Piece method: piece.
         *
         * @details Applies to a single chess piece instance.
         * @param Position Board position/index.
         * @return Result of the operation.
         */
        Piece(int,char,Position);
        /**
         * @brief Get color.
         * @details Applies to a single chess piece instance.
         * @return Color of the piece.
         */
        int getColor();
        /**
         * @brief Get the Symbol object
         * 
         * @return char 
         */
        char getSymbol();
        /**
         * @brief Get the Position object
         * 
         * @return Position 
         */
        Position getPosition();
        /**
         * @brief Set color.
         *
         * @details Applies to a single chess piece instance.
         * @param c Color to set.
         */
        void setColor(int c);
        /**
         * @brief Set the Symbol object
         * @param s Symbol to set
         */
        void setSymbol(char s);
        /**
         * @brief Set position.
         *
         * @details Applies to a single chess piece instance.
         * @param setPos Board position/index.
         */
        void setPosition(Position setPos);
        /**
         * @brief Abstract method to check if a move is valid.
         * 
         * @param newPos New position to move to.
         * @param board Reference to the board.
         * @return true If the move is valid.
         * @return false If the move is not valid.
         */
        virtual bool canMove(Position newPos, Board &board);
};