#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <random>


#include "game_engine.hpp"

//////////////////////////////// GameEngine ////////////////////////////////
GameEngine::GameEngine(int board_x, int board_y)
: m_moveIsOk(true), 
  m_max_x(board_x), 
  m_max_y(board_y), 
  m_snake(board_x, board_y),
  m_food(board_x, board_y, &m_snake),
  m_board(board_x, board_y, &m_snake, &m_food), 
  m_lastDirInput(GameUtilities::Dir::RIGHT),
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
                m_lastDirInput = GameUtilities::Dir::UP;
                break;
            case 's':
                m_lastDirInput = GameUtilities::Dir::DOWN;
                break;
            case 'a':
                m_lastDirInput = GameUtilities::Dir::LEFT;
                break;
            case 'd':
                m_lastDirInput = GameUtilities::Dir::RIGHT;
                break;
            default:         
                m_lastDirInput = GameUtilities::Dir::OTHER;
                break;    
        }
    }
}










































//////////////////////////////// Food ////////////////////////////////

