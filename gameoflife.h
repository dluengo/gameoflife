#ifndef __GAMEOFLIFE__
#define __GAMEOFLIFE__

#include <vector>

class GameOfLife {
private:
    int m_width {};
    int m_height {};
    std::vector<std::vector<bool>> m_universe;

    int get_number_neighbors(int x, int y);
    void print_universe();
    bool is_in_range(int x, int y);

public:
    GameOfLife(int height, int width);
    void do_step();
    void run();
    int setCell(int x, int y, bool val);
    bool isCellAlive(int x, int y);
    int getHeight();
    int getWidth();
    void printGame();
};

#endif
