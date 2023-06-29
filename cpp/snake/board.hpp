#pragma once

#include <map>

#include "snake.hpp"
#include "food.hpp"

class Board
{
public:
    Board(int board_x, int board_y, Snake* snake, Food* food);
    ~Board();
    char& GetPoint(Point point);
    bool UpdateBoard(Snake* snake, Food* food);
private:
    int m_board_x;
    int m_board_y;
    std::map<Point, char> m_board;
    Point m_lastHeadSnake;
    Point m_lastTailSnake;
    Point m_pointOfFood;
};