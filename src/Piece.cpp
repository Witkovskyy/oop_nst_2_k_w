#pragma once
#include <iostream>
#include "Piece.h"
using namespace std;


Piece::Piece(int c,char s, Position pos){
    color =c;
    symbol =s;
    current_position = pos;
}
int Piece::getColor(){
     return color;
}
char Piece::getSymbol(){
    return symbol;
}
Position Piece::getPosition(){
    return current_position;
}
void Piece::setColor(int c){
    color = c;
}
void Piece::setSymbol(char s){
    symbol = s;
}
void Piece::setPosition(Position setPos){
    current_position = setPos;
}

