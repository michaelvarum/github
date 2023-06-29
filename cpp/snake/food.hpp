#pragma once

#include <utility>

#include "snake.hpp"
#include "point.hpp"

class Food
{
public:
    Food(int x, int y, Snake* snake);
    Point GetPointOfFood();
    bool IsFoodPos(Point point);
    void UpdateFoodPos();
private:
    Point GetRandomPoint();
    int m_max_x;
    int m_max_y;
    Snake* m_snake;
    std::pair<Point, char> m_food;
};