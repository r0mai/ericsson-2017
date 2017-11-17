#include "Gui.h"

#include <iostream>
#include <sstream>
#include <iomanip>


namespace evil {

namespace {

sf::Color owner_colors[] = {
	sf::Color::White,
	sf::Color(60, 90, 180),
	sf::Color(200, 255, 180),
	sf::Color(140, 240, 40),
	sf::Color(255, 20, 40),
	sf::Color(255, 200, 200),
	sf::Color(255, 240, 240),
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
				case sf::Keyboard::Up: dir_ = Direction::kUp; break;
				case sf::Keyboard::Down: dir_ = Direction::kDown; break;
				case sf::Keyboard::Right: dir_ = Direction::kRight; break;
				case sf::Keyboard::Left: dir_ = Direction::kLeft; break;
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

Direction Gui::getDirection() const {
	return dir_;
}

void Gui::updateStatus() {
	std::stringstream ss;
	float coverage = model_.getCoverage() / 80.0f;
	ss << "Level " << model_.getLevel();
	ss << " - Tick " << model_.getTick();
	ss << " - Coverage " << std::setprecision(2) << coverage << "%";
	window_.setTitle(ss.str());
}

void Gui::close() {
	window_.close();
}

} // namespace evil
