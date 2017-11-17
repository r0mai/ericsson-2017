#pragma once

#include <SFML/Graphics.hpp>

#include "Protocol.h"
#include "Model.h"

namespace evil {

class Gui {
public:
	bool init();
	void close();
	bool update();
	void updateStatus();
	void draw();
	void setModel(Model model);
	bool pollEvent(sf::Event& event);
	Direction getDirection() const;
	void setNextPos(const Pos& pos);

private:
	static constexpr int map_w = 100;
	static constexpr int map_h = 80;

	static constexpr int window_w = 1024 * 2;
	static constexpr int window_h = 768 * 2;

	static constexpr float cell_w = (float)window_w / map_w;
	static constexpr float cell_h = (float)window_h / map_h;

	void drawCell(const Pos& pos, const Cell& cell);
	void drawCell(const Pos& pos, sf::Color color);
	void drawDot(const Pos& pos, sf::Color color);
	Pos windowToPos(int x, int y) const;

	sf::RenderWindow window_ {sf::VideoMode(window_w, window_h), "Window"};

	Model model_;
	Direction dir_ = Direction::kNone;
	Pos mouse_pos_;
	Pos next_pos_;
};

} // namespace evil
