#pragma once

#include <deque>
#include <utility>
#include <set>
#include "game_utilities.hpp"
#include "point.hpp"


class Snake
{
public:
    Snake(int x, int y);
    ~Snake();
    void Move(GameUtilities::Dir dir);
    Point GetHeadPoint();
    Point GetTailPoint();
    bool IsSnakePos(Point point);
    void UpdateState(GameUtilities::Dir dir);
    void UpdatSnakeTail(Point tail);
    void UpdatSnakeHead(Point head);
    void WasEaten(Point point);
private:
    std::deque<std::pair<Point, char>> m_body;
    int m_size;
    GameUtilities::Dir m_state;
    std::set<Point> m_snakePositions;
};