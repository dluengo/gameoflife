#include <vector>
#include <random>
#include <iostream>

#include <unistd.h>

#include "gameoflife.h"

int GameOfLife::get_number_neighbors(int x, int y) {
    int neighbors {0};
    for (int i = x-1; i < x+2; i++) {
        for (int j = y-1; j < y+2; j++) {
            if (i < 0 || i >= m_height || j < 0 || j >= m_width || (i == x && j == y)) {
                continue;
            }

            if (m_universe[i][j] == true) {
                neighbors++;
            }
        }
    }

    return neighbors;
}

void GameOfLife::print_universe() {
    for (int i = 0; i < m_height; i++) {
        std::cout << "[";
        for (int j = 0; j < m_width; j++) {
            char c = m_universe[i][j]? '*': ' ';

            if (j == m_width-1) {
                std::cout << c << "]\n";
            } else {
                std::cout << c << ",";
            }
        }
    }

    std::cout << "---------------------------------------\n";
}

bool GameOfLife::is_in_range(int x, int y) {
	if (x < 0 || x >= m_height || y < 0 || y >= m_width) {
		return false;
	}

	return true;
}

GameOfLife::GameOfLife(int height, int width):
	m_width(width),
	m_height(height),
	m_universe(std::vector<std::vector<bool>>(height, std::vector<bool>(width, false))) {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 10);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int roll = distribution(generator);

            if (roll < 5) {
                m_universe[i][j] = true;
            }
        }
    }
}

void GameOfLife::do_step() {
    std::vector<std::vector<bool>> next_state;

    next_state = std::vector<std::vector<bool>>(m_height, std::vector<bool>(m_width, false));
    for (int i = 0; i < m_height; i++) {
        for (int j = 0; j < m_width; j++) {
            int n_neighbors { get_number_neighbors(i, j) };

            if (m_universe[i][j]) {
                if (n_neighbors < 2 || n_neighbors > 3) {
                    next_state[i][j] = false;
                }
                else {
                    next_state[i][j] = true;
                }
            } else {
                if (n_neighbors == 3) {
                    next_state[i][j] = true;
                }
            }
        }
    }

    m_universe = next_state;
}

void GameOfLife::run() {
    int step {0};
    do {
        system("clear");
        std::cout << "Step: " << step << '\n';
        print_universe();
        do_step();
        step++;
        sleep(1);
    } while (true);

}

int GameOfLife::setCell(int x, int y, bool val) {
    if (!is_in_range(x, y)) {
    	return -1;
    }

    m_universe[x][y] = val;
    return 0;
}

bool GameOfLife::isCellAlive(int x, int y) {
	if (!is_in_range(x,y)) {
		return false;
	}

	return m_universe[x][y];
}

int GameOfLife::getHeight() {
	return m_height;
}

int GameOfLife::getWidth() {
	return m_width;
}

void GameOfLife::printGame() {
	print_universe();
}
