#include <functional>
#include <random>

#include "food.hpp"
#include "snake.hpp"

Food::Food(int x, int y, Snake* snake)
: m_max_x(x), m_max_y(y), m_snake(snake), m_food(GetRandomPoint(), '$')
{}

Point Food::GetRandomPoint()
{
    Point food_pos(0, 0);
    do
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<int> distX(1, m_max_x - 2);  // Adjust range to avoid borders
        std::uniform_int_distribution<int> distY(1, m_max_y - 2);  // Adjust range to avoid borders

        food_pos = Point(distX(gen), distY(gen));
    } while (m_snake->IsSnakePos(food_pos));

    return food_pos;
}

Point Food::GetPointOfFood()
{
    return m_food.first;
}

bool Food::IsFoodPos(Point point)
{
    return (point == m_food.first);
}

void Food::UpdateFoodPos()
{
    m_food.first = GetRandomPoint();
}