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


void Gui::drawCell(const Pos& pos, sf::Color color) {
	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(cell_w, cell_h));
	rectangle.setFillColor(color);
	rectangle.setPosition(pos.col * cell_w, pos.row * cell_h);
	window_.draw(rectangle);
}

void Gui::drawDot(const Pos& pos, sf::Color color) {
    sf::CircleShape circ;
    float r = 3.0f;

    circ.setRadius(r);
    circ.setOrigin(r, r);
	circ.setFillColor(color);
	circ.setPosition((pos.col + 0.5f) * cell_w, (pos.row + 0.5f) * cell_h);
	window_.draw(circ);
}

void Gui::drawCell(const Pos& pos, const Cell& cell) {
	auto color = owner_colors[cell.color];
	drawCell(pos, color);
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
		if (event.type == sf::Event::MouseMoved) {
			auto& ev = event.mouseMove;
			mouse_pos_ = windowToPos(ev.x, ev.y);
		}
	}

	return window_.isOpen();
}

Pos Gui::windowToPos(int x, int y) const {
	Pos pos{int(y / cell_h), int(x / cell_w)};
	return pos;
}

void Gui::draw() {
	window_.clear(sf::Color::Black);

	auto& grid = model_.getGrid();

	for (int r=0; r < grid.width(); ++r) {
		for (int c=0; c < grid.height(); ++c) {
			auto& cell = grid.at(r, c);
			drawCell({r, c}, cell);
		}
	}

	drawDot(next_pos_, sf::Color::Black);
	drawDot(neighbor(next_pos_, dir_), sf::Color::Black);

	drawCell(mouse_pos_, sf::Color(50, 230, 250, 100));
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

void Gui::setNextPos(const Pos& pos) {
	next_pos_ = pos;
}

} // namespace evil
