#include "gui.h"

#include <iostream>

namespace evil {

namespace {

sf::Color owner_colors[] = {
	sf::Color::White,
	sf::Color::Blue,
	sf::Color(200, 250, 200),
	sf::Color::Green,
	sf::Color::Red,
	sf::Color(255, 200, 200)
};

} // anonymous namespace

void Gui::drawCell(int row_idx, int coll_idx, const Cell& cell) {
	sf::RectangleShape rectangle;
	auto color = owner_colors[cell.color];
	rectangle.setSize(sf::Vector2f(cell_w, cell_h));
	rectangle.setFillColor(color);
	rectangle.setPosition(coll_idx * cell_w, row_idx * cell_h);

	window_.draw(rectangle);
}


bool Gui::init() {
	return true;
}

void Gui::setModel(Model model) {
	model_ = std::move(model);
}

bool Gui::update() {
	sf::Event event;
	while (window_.pollEvent(event))
	{
		// "close requested" event: we close the window_
		if (event.type == sf::Event::Closed) {
			window_.close();
		}

		if (event.type == sf::Event::KeyPressed) {
			switch (event.key.code) {
				default: break;
				case sf::Keyboard::Up: last_direction_ = Direction::UP; break;
				case sf::Keyboard::Down: last_direction_ = Direction::DOWN; break;
				case sf::Keyboard::Right: last_direction_ = Direction::RIGHT; break;
				case sf::Keyboard::Left: last_direction_ = Direction::LEFT; break;
				case sf::Keyboard::Escape: window_.close(); break;
			}
		}
	}

	return window_.isOpen();
}

void Gui::draw() {
	window_.clear(sf::Color::Black);

	auto& grid = model_.getGrid();

	for (int r=0; r < grid.width(); ++r) {
		for (int c=0; c < grid.height(); ++c) {
			auto& cell = grid.at(r, c);
			drawCell(r, c, cell);
		}
	}

	window_.display();
}

bool Gui::pollEvent(sf::Event& event) {
	return window_.pollEvent(event);
}

Direction Gui::getLastDirection() const {
	return last_direction_;
}

} // namespace evil
