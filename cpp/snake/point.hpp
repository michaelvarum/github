#pragma once

class Point
{
public:
    Point(int x, int y);
    bool operator<(const Point& other) const;
    Point& operator+=(const Point& other);
    Point& operator-=(const Point& other);
    bool operator==(const Point& other) const;
    int GetX() const; // Get function for x
    int GetY() const; // Get function for y
    Point& operator=(const Point& other);
private:
    int m_x;
    int m_y;
};
