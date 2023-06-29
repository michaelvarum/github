#include <map>
#include <deque>
#include <utility>
#include <random>
#include <set>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>

#include "game_utilities.hpp"
#include "point.hpp"
#include "snake.hpp"
#include "food.hpp"
#include "board.hpp"


using namespace std;

class Point;
class Board;
class Snake;
class Food;
class GameEngine;


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
    std::atomic<GameUtilities::Dir> m_lastDirInput;
    thread m_inputThread;
};