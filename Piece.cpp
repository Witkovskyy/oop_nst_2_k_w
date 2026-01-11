/**
 * @brief Get color.
 *
 * @details Applies to a single chess piece instance.
 * @return Requested value.
 */


﻿#include <iostream>
#include "Piece.h"
using namespace std;


class Board;

/**
 * @brief Get color.
 *
 * @details Applies to a single chess piece instance.
 * @return Requested value.
 */
Piece::Piece(int c, char s, Position pos) {
    color = c;
    symbol = s;
    current_position = pos;
}
int Piece::getColor() {
    return color;
}
char Piece::getSymbol() {
    return symbol;
}
/**
 * @brief Get position.
 *
 * @details Applies to a single chess piece instance.
 * @return Requested value.
 */
Position Piece::getPosition() {
    return current_position;
}
void Piece::setColor(int c) {
    color = c;
}
/**
 * @brief Set symbol.
 *
 * @details Applies to a single chess piece instance.
 * @param s Parameter.
 */
void Piece::setSymbol(char s) {
    symbol = s;
}
void Piece::setPosition(Position setPos) {
    current_position = setPos;
}
/**
 * @brief Check whether this piece can move to the given square (piece rules only).
 *
 * @details Validates movement pattern for the specific piece type. Additional game rules (e.g., leaving king in check) are handled elsewhere.
 * @param newPos Board position/index.
 * @param board Board state to operate on.
 * @return True if the condition holds; otherwise false.
 */
bool Piece::canMove(Position newPos, Board &board) {return false;}
