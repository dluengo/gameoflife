#ifndef __GAMEOFLIFEVIEW__
#define __GAMEOFLIFEVIEW__

#include <gtkmm.h>

#include "gameoflife.h"

class GameOfLifeView : public Gtk::Window {
protected:
	GameOfLife m_game;
	Gtk::Grid *m_board_grid;
	Gtk::Button *m_run_button;
	bool m_game_is_running;

	void on_run_button_clicked();
	void on_new_game_button_clicked();

public:
	GameOfLifeView(GameOfLife game);
	~GameOfLifeView();
	GameOfLife * getGame();
	bool isGameRunning();
	void refreshView();
};

#endif
