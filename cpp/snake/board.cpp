#include "board.hpp"

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
