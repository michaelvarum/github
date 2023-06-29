#include "point.hpp"

Point::Point(int x, int y)
: m_x(x), m_y(y)
{}


bool Point::operator<(const Point& other) const
{
    if (m_x < other.m_x)
        return true;
    if (m_x > other.m_x)
        return false;
    return m_y < other.m_y;
}

Point& Point::operator+=(const Point& other)
{
    m_x += other.m_x;
    m_y += other.m_y;
    return *this;
}

Point& Point::operator-=(const Point& other)
{
    m_x -= other.m_x;
    m_y -= other.m_y;
    return *this;
}

bool Point::operator==(const Point& other) const
{
    return (this->m_x == other.m_x && this->m_y == other.m_y);
}

int Point::GetX() const
{
    return m_x;
}

int Point::GetY() const
{
    return m_y;
}

Point& Point::operator=(const Point& other)
{
    if (this == &other) // Check for self-assignment
        return *this;

    m_x = other.m_x;
    m_y = other.m_y;

    return *this;
}