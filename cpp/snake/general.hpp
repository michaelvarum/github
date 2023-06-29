#include <map>
#include <deque>
#include <utility>
#include <random>
#include <set>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>

using namespace std;

class Point;
class Board;
class Snake;
class Food;
class GameEngine;

enum Dir
{
    RIGHT,
    LEFT,
    UP,
    DOWN,
    OTHER
};

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
    map<Point, char> m_board;
    Point m_lastHeadSnake;
    Point m_lastTailSnake;
    Point m_pointOfFood;
};

class Snake
{
public:
    Snake(int x, int y);
    ~Snake();
    void Move(Dir dir);
    Point GetHeadPoint();
    Point GetTailPoint();
    bool IsSnakePos(Point point);
    void UpdateState(Dir dir);
    void UpdatSnakeTail(Point tail);
    void UpdatSnakeHead(Point head);
    void WasEaten(Point point);
private:
    deque<pair<Point, char>> m_body;
    int m_size;
    Dir m_state;
    set<Point> m_snakePositions;
};



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
    pair<Point, char> m_food;
};


class GameEngine
{
public:
    GameEngine(int board_x, int board_y);
    ~GameEngine();
    void Run();
    void PrintObjects();
    void GetDirectionFromUser();
private:
    std::atomic<bool> m_moveIsOk;
    int m_max_x;
    int m_max_y;
    Snake m_snake;
    Food m_food;
    Board m_board;
    std::atomic<Dir> m_lastDirInput;
    thread m_inputThread;
};


