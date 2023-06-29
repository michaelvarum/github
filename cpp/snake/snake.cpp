#include <iostream>
#include "snake.hpp"

Snake::Snake(int x, int y)
: m_body(), m_size(5), m_state(GameUtilities::RIGHT), m_snakePositions()
{
    for (int i = 0; i < m_size; ++i)
    {
        m_body.push_back(std::make_pair(Point(x / 2 - i, y / 2), '@'));
        m_snakePositions.insert(Point(x / 2 - i, y / 2));
    }
}

Snake::~Snake()
{}


void Snake::Move(GameUtilities::Dir dir)
{
    UpdateState(dir);
    switch (m_state)
    {
        case GameUtilities::RIGHT:
        {
            Point new_head = m_body.front().first;
            new_head += Point(1, 0);
            Point prev_tail = m_body.back().first;
            UpdatSnakeTail(prev_tail);
            std::cout << "new_headnew_head.GetX()" << new_head.GetX() << std::endl;
            std::cout << "new_headnew_head.GetY()" << new_head.GetY() << std::endl;
            std::cout << "prev_tail.GetX()" << prev_tail.GetX() << std::endl;
            std::cout << "prev_tail.GetY()" << prev_tail.GetY() << std::endl;
            m_body.pop_back();
            m_body.push_front(std::make_pair(new_head, '@'));

            break;
        }
        case GameUtilities::LEFT:
        {
            Point new_head = m_body.front().first;
            new_head -= Point(1, 0);
            Point prev_tail = m_body.back().first;
            UpdatSnakeTail(prev_tail);
            std::cout << "new_headnew_head.GetX()" << new_head.GetX() << std::endl;
            std::cout << "new_headnew_head.GetY()" << new_head.GetY() << std::endl;
            std::cout << "prev_tail.GetX()" << prev_tail.GetX() << std::endl;
            std::cout << "prev_tail.GetY()" << prev_tail.GetY() << std::endl;
            m_body.pop_back();
            m_body.push_front(std::make_pair(new_head, '@'));

            break;
        }
        case GameUtilities::UP:
        {
            Point new_head = m_body.front().first;
            new_head -= Point(0, 1);
            Point prev_tail = m_body.back().first;
            UpdatSnakeTail(prev_tail);
            std::cout << "new_headnew_head.GetX()" << new_head.GetX() << std::endl;
            std::cout << "new_headnew_head.GetY()" << new_head.GetY() << std::endl;
            std::cout << "prev_tail.GetX()" << prev_tail.GetX() << std::endl;
            std::cout << "prev_tail.GetY()" << prev_tail.GetY() << std::endl;
            m_body.pop_back();
            m_body.push_front(std::make_pair(new_head, '@'));

            break;
        }
        case GameUtilities::DOWN:
        {
            Point new_head = m_body.front().first;
            new_head += Point(0, 1);
            Point prev_tail = m_body.back().first;
            UpdatSnakeTail(prev_tail);
            std::cout << "new_headnew_head.GetX()" << new_head.GetX() << std::endl;
            std::cout << "new_headnew_head.GetY()" << new_head.GetY() << std::endl;
            std::cout << "prev_tail.GetX()" << prev_tail.GetX() << std::endl;
            std::cout << "prev_tail.GetY()" << prev_tail.GetY() << std::endl;
            m_body.pop_back();
            m_body.push_front(std::make_pair(new_head, '@'));

            break;
        }
    }
}


Point Snake::GetHeadPoint()
{
    return m_body.front().first;
}

Point Snake::GetTailPoint()
{
    return m_body.back().first;
}

bool Snake::IsSnakePos(Point point)
{
    return m_snakePositions.find(point) != m_snakePositions.end();
}

void Snake::UpdateState(GameUtilities::Dir dir)
{
    if (dir == GameUtilities::OTHER || 
       (dir == GameUtilities::UP && m_state == GameUtilities::DOWN) || 
       (dir == GameUtilities::DOWN && m_state == GameUtilities::UP) || 
       (dir == GameUtilities::LEFT && m_state == GameUtilities::RIGHT) || 
       (dir == GameUtilities::RIGHT && m_state == GameUtilities::LEFT))
    {
        return;
    }

    m_state = dir;
}

void Snake::UpdatSnakeTail(Point tail)
{
    m_snakePositions.erase(tail);
}

void Snake::UpdatSnakeHead(Point head)
{
    m_snakePositions.insert(head);
}

void Snake::WasEaten(Point point)
{
    m_body.push_back(std::make_pair(point, '@'));
}