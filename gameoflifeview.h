#ifndef __GAMEOFLIFEVIEW__
#define __GAMEOFLIFEVIEW__

#include <gtkmm.h>

#include "gameoflife.h"

class GameOfLifeView {
protected:
	Gtk::Window *m_toplevel_window;
	Gtk::Grid *m_board_grid;
	Gtk::Button *m_run_button;
	Gtk::Button *m_new_game_button;
	Gtk::Entry *m_height_entry;
	Gtk::Entry *m_width_entry;
	Gtk::Entry *m_speed_entry;

	GameOfLife m_game;
	bool m_game_is_running;
	int m_game_speed;

	void on_run_button_clicked();
	void on_new_game_button_clicked();
	void run_game();
	void pause_game();

public:
	GameOfLifeView(GameOfLife game);
	~GameOfLifeView();
	GameOfLife* getGame();
	bool isGameRunning();
	void refreshView();
	Gtk::Window* getWindow();
};

#endif
