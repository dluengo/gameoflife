#include <gtkmm.h>

#include "gameoflifeview.h"

int main(int argc, char *argv[]) {

	auto app = Gtk::Application::create("org.gtkmm.gameoflife"); // @suppress("Invalid arguments")
	GameOfLife game = GameOfLife(10, 10);
	GameOfLifeView game_view = GameOfLifeView(game);

	return app->run(game_view); // @suppress("Method cannot be resolved")
}
