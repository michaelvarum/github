#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <random>

#include "general.hpp"



//////////////////////////////// GameEngine ////////////////////////////////
GameEngine::GameEngine(int board_x, int board_y)
: m_moveIsOk(true), 
  m_max_x(board_x), 
  m_max_y(board_y), 
  m_snake(board_x, board_y),
  m_food(board_x, board_y, &m_snake),
  m_board(board_x, board_y, &m_snake, &m_food), 
  m_lastDirInput(RIGHT),
  m_inputThread()
{}

GameEngine::~GameEngine()
{}

void GameEngine::Run()
{
    m_inputThread = thread(&GameEngine::GetDirectionFromUser, std::ref(*this));
    while (m_moveIsOk)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        std::system("clear");
        m_snake.Move(m_lastDirInput);
        m_moveIsOk = m_board.UpdateBoard(&m_snake, &m_food);
        PrintObjects();
    }
}

void GameEngine::PrintObjects()
{
    for (int y = 0; y < m_max_y; ++y)
    {
        for (int x = 0; x < m_max_x; ++x)
        {
            cout << m_board.GetPoint({x, y});
        }
        cout << endl;
    }
}

void GameEngine::GetDirectionFromUser() 
{
    char input;
    std::cout << "Enter direction (w for up, s for down, a for left, d for right): ";

    while(m_moveIsOk)
    {
        std::cin >> input;
        switch (input) 
        {
            case 'w':
                m_lastDirInput = UP;
                break;
            case 's':
                m_lastDirInput = DOWN;
                break;
            case 'a':
                m_lastDirInput = LEFT;
                break;
            case 'd':
                m_lastDirInput = RIGHT;
                break;
            default:         
                m_lastDirInput = OTHER;
                break;    
        }
    }
}
































//////////////////////////////// Board ////////////////////////////////

Board::Board(int board_x, int board_y, Snake* snake, Food* food)
: m_board_x(board_x), 
  m_board_y(board_y), 
  m_board(), 
  m_lastHeadSnake(snake->GetHeadPoint()),
  m_lastTailSnake(snake->GetTailPoint()),
  m_pointOfFood(food->GetPointOfFood())
{

    for (int y = 0; y < board_y; ++y)
    {
        for (int x = 0; x < board_x; ++x)
        {
            if (y == 0 || y == board_y - 1)
            {
                m_board[Point(x, y)] = '*';
            }
            else if (snake->IsSnakePos(Point(x, y)))
            {
                m_board[Point(x, y)] = '@';
            }
            else
            {
                if (x == 0 || x == board_x - 1)
                {
                    m_board[Point(x, y)] = '*';
                }
                else
                {
                    m_board[Point(x, y)] = ' ';
                }
            }
        }
    }
}

Board::~Board()
{}


char& Board::GetPoint(Point point)
{
    return m_board[point];
}

bool Board::UpdateBoard(Snake* snake, Food* food)
{
    if (m_board[snake->GetHeadPoint()] == '*' || snake->IsSnakePos(snake->GetHeadPoint()))
    {
        m_board[food->GetPointOfFood()] = '$';
        m_board[m_lastTailSnake] = ' ';
        m_board[snake->GetHeadPoint()] = '@';
        snake->UpdatSnakeHead(snake->GetHeadPoint());
        return false;
    }
    else if (food->IsFoodPos(snake->GetHeadPoint()))
    {
        food->UpdateFoodPos();
        snake->WasEaten(m_lastTailSnake);
    }

    m_board[food->GetPointOfFood()] = '$';
    m_board[m_lastTailSnake] = ' ';
    m_board[snake->GetHeadPoint()] = '@';
    snake->UpdatSnakeHead(snake->GetHeadPoint());
    m_lastTailSnake = snake->GetTailPoint();
    m_lastHeadSnake = snake->GetHeadPoint();    // maybe the variable is redandent
    return true;
}





























//////////////////////////////// Snake ////////////////////////////////



Snake::Snake(int x, int y)
: m_body(), m_size(5), m_state(RIGHT), m_snakePositions()
{
    for (int i = 0; i < m_size; ++i)
    {
        m_body.push_back(make_pair(Point(x / 2 - i, y / 2), '@'));
        m_snakePositions.insert(Point(x / 2 - i, y / 2));
    }
}

Snake::~Snake()
{}


void Snake::Move(Dir dir)
{
    UpdateState(dir);
    switch (m_state)
    {
        case RIGHT:
        {
            Point new_head = m_body.front().first;
            new_head += Point(1, 0);
            Point prev_tail = m_body.back().first;
            UpdatSnakeTail(prev_tail);
            cout << "new_headnew_head.GetX()" << new_head.GetX() << endl;
            cout << "new_headnew_head.GetY()" << new_head.GetY() << endl;
            cout << "prev_tail.GetX()" << prev_tail.GetX() << endl;
            cout << "prev_tail.GetY()" << prev_tail.GetY() << endl;
            m_body.pop_back();
            m_body.push_front(make_pair(new_head, '@'));

            break;
        }
        case LEFT:
        {
            Point new_head = m_body.front().first;
            new_head -= Point(1, 0);
            Point prev_tail = m_body.back().first;
            UpdatSnakeTail(prev_tail);
            cout << "new_headnew_head.GetX()" << new_head.GetX() << endl;
            cout << "new_headnew_head.GetY()" << new_head.GetY() << endl;
            cout << "prev_tail.GetX()" << prev_tail.GetX() << endl;
            cout << "prev_tail.GetY()" << prev_tail.GetY() << endl;
            m_body.pop_back();
            m_body.push_front(make_pair(new_head, '@'));

            break;
        }
        case UP:
        {
            Point new_head = m_body.front().first;
            new_head -= Point(0, 1);
            Point prev_tail = m_body.back().first;
            UpdatSnakeTail(prev_tail);
            cout << "new_headnew_head.GetX()" << new_head.GetX() << endl;
            cout << "new_headnew_head.GetY()" << new_head.GetY() << endl;
            cout << "prev_tail.GetX()" << prev_tail.GetX() << endl;
            cout << "prev_tail.GetY()" << prev_tail.GetY() << endl;
            m_body.pop_back();
            m_body.push_front(make_pair(new_head, '@'));

            break;
        }
        case DOWN:
        {
            Point new_head = m_body.front().first;
            new_head += Point(0, 1);
            Point prev_tail = m_body.back().first;
            UpdatSnakeTail(prev_tail);
            cout << "new_headnew_head.GetX()" << new_head.GetX() << endl;
            cout << "new_headnew_head.GetY()" << new_head.GetY() << endl;
            cout << "prev_tail.GetX()" << prev_tail.GetX() << endl;
            cout << "prev_tail.GetY()" << prev_tail.GetY() << endl;
            m_body.pop_back();
            m_body.push_front(make_pair(new_head, '@'));

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

void Snake::UpdateState(Dir dir)
{
    if (dir == OTHER || 
       (dir == UP && m_state == DOWN) || 
       (dir == DOWN && m_state == UP) || 
       (dir == LEFT && m_state == RIGHT) || 
       (dir == RIGHT && m_state == LEFT))
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
    m_body.push_back(make_pair(point, '@'));
}



























//////////////////////////////// Point ////////////////////////////////
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












//////////////////////////////// Food ////////////////////////////////

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