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

GameOfLifeView *_GAME;

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
		std::cout << __func__ << '\n';
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
	_GAME->getGame()->do_step();
	_GAME->refreshView();
}

// This handler handles both the run and pause actions.
void GameOfLifeView::on_run_button_clicked() {
	if (!m_game_is_running) {
		run_game();
	} else {
		pause_game();
	}
}

void GameOfLifeView::on_new_game_button_clicked() {
	if (m_game_is_running) {
		pause_game();
	}

	for (int i = 0; i < m_game.getHeight(); i++) {
		for (int j = 0; j < m_game.getWidth(); j++) {
			CellView *cellview = dynamic_cast<CellView *>(m_board_grid->get_child_at(i, j));
			m_board_grid->remove(*cellview);
		}
	}

	int new_height{ std::stoi(m_height_entry->get_text()) }; // @suppress("Invalid arguments")
	int new_width{ std::stoi(m_width_entry->get_text()) }; // @suppress("Invalid arguments")
	m_game_speed = std::stoi(m_speed_entry->get_text()) * 1000 ; // @suppress("Invalid arguments")

	m_game = GameOfLife(new_height, new_width);

	for (int i = 0; i < new_height; i++) {
		for (int j = 0; j < new_width; j++) {
			CellView *cellview = new CellView(i, j, &m_game);
			Gtk::manage(cellview);
			cellview->signal_evolve().connect(sigc::mem_fun(*cellview, &CellView::on_evolve));
			m_board_grid->attach(*cellview, i, j);
		}
	}

	m_toplevel_window->show_all();
}

void GameOfLifeView::run_game() {
	// Set SIGALRM handler.
	struct sigaction sigact;
	sigact.sa_handler = alarm_handler; // @suppress("Field cannot be resolved")
	sigemptyset(&sigact.sa_mask); // @suppress("Function cannot be resolved") // @suppress("Field cannot be resolved")
	sigact.sa_flags = 0; // @suppress("Field cannot be resolved")
	int err = sigaction(SIGALRM, &sigact, NULL); // @suppress("Symbol is not resolved")
	if (err) {
		std::cout << "Error setting SIGALRM handler\n";
	}

	// Set timer. It'll generate SIGALRM when it goes off and resume the game.
	struct itimerval interval;
	interval.it_interval.tv_sec = 0;
	interval.it_interval.tv_usec = m_game_speed;
	interval.it_value.tv_sec = 0;
	interval.it_value.tv_usec = m_game_speed;
	err = setitimer(ITIMER_REAL, &interval, NULL);
	if (err) {
		std::cout << "Error setting timer\n";
	}

	m_run_button->set_label("Pause");
	m_game_is_running = true;
}

void GameOfLifeView::pause_game() {
	struct itimerval interval;
	interval.it_interval.tv_sec = 0;
	interval.it_interval.tv_usec = 0;
	interval.it_value.tv_sec = 0;
	interval.it_value.tv_usec = 0;
	int err = setitimer(ITIMER_REAL, &interval, NULL);
	if (err) {
		std::cout << "Error Disarming alarm\n";
	}

	m_run_button->set_label("Run");
	m_game_is_running = false;
}

GameOfLifeView::GameOfLifeView(GameOfLife game)
: m_game(game),
  m_game_is_running(false),
  m_game_speed(500000)
{
	_GAME = this;

	// Load the GUI description from file.
	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("gameoflife.glade"); // @suppress("Invalid arguments")
	builder->get_widget("toplevel_window", m_toplevel_window);

	// Set up the grid that will contain all the cells.
	builder->get_widget("board_grid", m_board_grid);
	for (int i = 0; i < m_game.getHeight(); i++) {
		for (int j = 0; j < m_game.getWidth(); j++) {
			CellView *cellview = new CellView(i, j, &m_game);
			Gtk::manage(cellview);

			cellview->signal_evolve().connect(sigc::mem_fun(*cellview, &CellView::on_evolve));
			m_board_grid->attach(*cellview, i, j);
		}
	}

	// Connect the buttons to the proper handlers.
	builder->get_widget("run_button", m_run_button);
	builder->get_widget("new_game_button", m_new_game_button);
	m_run_button->signal_clicked().connect(sigc::mem_fun(*this, &GameOfLifeView::on_run_button_clicked));
	m_new_game_button->signal_clicked().connect(sigc::mem_fun(*this, &GameOfLifeView::on_new_game_button_clicked));

	builder->get_widget("height_entry", m_height_entry);
	builder->get_widget("width_entry", m_width_entry);
	builder->get_widget("speed_entry", m_speed_entry);
	m_height_entry->set_text(std::to_string(m_game.getHeight())); // @suppress("Invalid arguments")
	m_width_entry->set_text(std::to_string(m_game.getWidth())); // @suppress("Invalid arguments")
	m_speed_entry->set_text(std::to_string(m_game_speed / 1000)); // @suppress("Invalid arguments")

	m_toplevel_window->show_all();
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

Gtk::Window * GameOfLifeView::getWindow() {
	return this->m_toplevel_window;
}
