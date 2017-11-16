#pragma once

#include <SFML/Graphics.hpp>
#include "protocol/Response.capnp.h"

namespace evil {

class Gui {
public:

	bool Init();
	void DrawGameState(protocol::Response::Reader& response_reader);
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
	void DrawEnemy(protocol::Enemy::Reader& enemy);
	void DrawUnit(protocol::Unit::Reader& unit);

	sf::RenderWindow window{sf::VideoMode(window_w, window_h), "Your window"};
};

} // namespace evil
