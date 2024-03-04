/*
 * gameoflifeview.cpp
 *
 *  Created on: Feb 26, 2024
 *      Author: dluengo
 */
#include <iostream>
#include <sigc++/sigc++.h>
#include <gtkmm.h>

#include <sys/time.h>

#include "gameoflifeview.h"

GameOfLifeView *GAME_VIEW;

class CellView: public Gtk::Button {
public:
	using SigEvolve = sigc::signal<void()>;

protected:
	int m_x, m_y;
	GameOfLife *m_ptr_game;
	SigEvolve m_signal_evolve;

public:
	CellView(int x, int y, GameOfLife * game): m_x(x), m_y(y), m_ptr_game(game) {
		this->signal_clicked().connect(sigc::mem_fun(*this, &CellView::on_cell_clicked));

		if (m_ptr_game->isCellAlive(m_x, m_y)) {
			this->set_label("●");
		} else {
			this->set_label(" ");
		}
	}
	~CellView() {}

	void on_cell_clicked() {
		if (m_ptr_game->isCellAlive(m_x, m_y)) {
			this->set_label(" ");
			m_ptr_game->setCell(m_x, m_y, false);
		} else {
			this->set_label("●");
			m_ptr_game->setCell(m_x, m_y, true);
		}
	}

	void on_evolve() {
		if (m_ptr_game->isCellAlive(m_x, m_y)) {
			this->set_label("●");
		} else {
			this->set_label(" ");
		}
	}

	SigEvolve signal_evolve() {
		return m_signal_evolve;
	}

	void refresh() {
		this->m_signal_evolve.emit();
	}
};

void alarm_handler(int signum) {
	GAME_VIEW->getGame()->do_step();
	GAME_VIEW->refreshView();
}

void GameOfLifeView::on_run_button_clicked() {
	if (m_game_is_running) {
		// Pause the game.
		std::cout << __func__ << " called\n";
		struct itimerval interval;
		interval.it_interval.tv_sec = 0;
		interval.it_interval.tv_usec = 0;
		interval.it_value.tv_sec = 0;
		interval.it_value.tv_usec = 0;
		int err = setitimer(ITIMER_REAL, &interval, NULL);
		if (err) {
			std::cout << "Error Disarming alarm\n";
		}

		// Prepare the run button.
		this->m_run_button->set_label("Run");

		this->m_game_is_running = false;
	} else {
		// Set SIGALRM handler.
		struct sigaction sigact;
		sigact.sa_handler = alarm_handler;
		sigemptyset(&sigact.sa_mask);
		sigact.sa_flags = 0;
		int err = sigaction(SIGALRM, &sigact, NULL);
		if (err) {
			std::cout << "Error setting SIGALRM handler\n";
		}

		// Set timer. It'll generate SIGALRM when it goes off.
		struct itimerval interval;
		interval.it_interval.tv_sec = 0;
		interval.it_interval.tv_usec = 500000;
		interval.it_value.tv_sec = 0;
		interval.it_value.tv_usec = 500000;
		err = setitimer(ITIMER_REAL, &interval, NULL);
		if (err) {
			std::cout << "Error setting timer\n";
		}

		this->m_run_button->set_label("Pause");
		this->m_game_is_running = true;
	}
}

void GameOfLifeView::on_new_game_button_clicked() {
	std::cout << __func__ << " called\n";
}

GameOfLifeView::GameOfLifeView(GameOfLife game):
		m_game(game),
		m_game_is_running(false) {
	GAME_VIEW = this;

	Gtk::Grid *toplevel_grid = new Gtk::Grid();
	Gtk::manage(toplevel_grid);
	m_board_grid = new Gtk::Grid();
	Gtk::manage(m_board_grid);
	Gtk::Grid *buttons_grid = new Gtk::Grid();
	Gtk::manage(buttons_grid);

	m_run_button = new Gtk::Button("Run");
	Gtk::manage(m_run_button);
	Gtk::Button *new_game_button = new Gtk::Button("New game");
	Gtk::manage(new_game_button);

	m_run_button->signal_clicked().connect(sigc::mem_fun(*this, &GameOfLifeView::on_run_button_clicked));
	new_game_button->signal_clicked().connect(sigc::mem_fun(*this, &GameOfLifeView::on_new_game_button_clicked));

	buttons_grid->attach(*m_run_button, 0, 0);
	buttons_grid->attach(*new_game_button, 1, 0);

	for (int i = 0; i < m_game.getHeight(); i++) {
		for (int j = 0; j < m_game.getWidth(); j++) {
			CellView *cellview = new CellView(i, j, &m_game);
			Gtk::manage(cellview);

			cellview->signal_evolve().connect(sigc::mem_fun(*cellview, &CellView::on_evolve));
			m_board_grid->attach(*cellview, i, j);
		}
	}

	toplevel_grid->attach(*m_board_grid, 0, 0);
	toplevel_grid->attach(*buttons_grid, 0, 1);

	this->add(*toplevel_grid);
	this->show_all();
}

GameOfLifeView::~GameOfLifeView(){
	;
}

GameOfLife * GameOfLifeView::getGame() {
	return &this->m_game;
}

bool GameOfLifeView::isGameRunning() {
	return this->m_game_is_running;
}

void GameOfLifeView::refreshView() {
	// Run through the buttons sending clicked signal.
	for (int i = 0; i < m_game.getHeight(); i++) {
		for (int j = 0; j < m_game.getWidth(); j++) {
			CellView *cellview = dynamic_cast<CellView*>(m_board_grid->get_child_at(i, j));
			if (!cellview) {
				std::cout << "Cast to CellView failed";
				return;
			}

			cellview->refresh();
		}
	}
}
