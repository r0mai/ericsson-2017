#pragma once

#include <SFML/Graphics.hpp>

#include "Model.h"

namespace evil {

class Gui {
public:

	bool Init();
	bool Update();
	void Draw();
	void SetModel(Model model);
	bool PollEvent(sf::Event& event);
private:
	static constexpr int map_w = 100;
	static constexpr int map_h = 80;

	static constexpr int window_w = 1024 * 2;
	static constexpr int window_h = 768 * 2;

	static constexpr float cell_w = (float)window_w / map_w;
	static constexpr float cell_h = (float)window_h / map_h;

	void DrawCell(int row_idx, int coll_idx, sf::Color color);
	void DrawCell(int row_idx, int coll_idx, int owner);
	void DrawAttack(int row_idx, int coll_idx, int owner);
	void DrawEnemy(const Enemy& enemy);
	void DrawUnit(const Unit& unit);

	sf::RenderWindow window{sf::VideoMode(window_w, window_h), "Your window"};

	Model model_;
};

} // namespace evil
