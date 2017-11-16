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

void Gui::DrawCell(int row_idx, int coll_idx, const Cell& cell) {
	sf::RectangleShape rectangle;
	auto color = owner_colors[cell.color];
	rectangle.setSize(sf::Vector2f(cell_w, cell_h));
	rectangle.setFillColor(color);
	rectangle.setPosition(coll_idx * cell_w, row_idx * cell_h);

	window.draw(rectangle);
}

void Gui::DrawAttack(int row_idx, int coll_idx, int owner) {
	// auto color = owner_colors[2];
	// color.a = 100;
	// DrawCell(row_idx, coll_idx, color);
}

void Gui::DrawEnemy(const Enemy& enemy) {
	// DrawCell(enemy.pos.row, enemy.pos.col, 3);
}

void Gui::DrawUnit(const Unit& unit) {
	// DrawCell(unit.pos.row, unit.pos.col, 2);
}

bool Gui::Init() {
//	while (window->isOpen()) {
//		sf::Event event;
//		while (window->pollEvent(event))
//		{
//			// "close requested" event: we close the window
//			if (event.type == sf::Event::Closed)
//				window->close();
//		}
//		window.clear(sf::Color::Black);
//		window.display();
//	}
	return true;
}

void Gui::SetModel(Model model) {
	model_ = std::move(model);
}

bool Gui::Update() {
	sf::Event event;
	while (window.pollEvent(event))
	{
		// "close requested" event: we close the window
		if (event.type == sf::Event::Closed) {
			window.close();
		}

		if (event.type == sf::Event::KeyPressed) {
			switch (event.key.code) {
				default: break;
				case sf::Keyboard::Up: last_direction_ = Direction::UP; break;
				case sf::Keyboard::Down: last_direction_ = Direction::DOWN; break;
				case sf::Keyboard::Right: last_direction_ = Direction::RIGHT; break;
				case sf::Keyboard::Left: last_direction_ = Direction::LEFT; break;
				case sf::Keyboard::Escape: window.close(); break;
			}
		}
	}

	return window.isOpen();
}

void Gui::Draw() {
	window.clear(sf::Color::Black);

	auto& grid = model_.getGrid();

	for (int r=0; r < grid.width(); ++r) {
		for (int c=0; c < grid.height(); ++c) {
			auto& cell = grid.at(r, c);
			DrawCell(r, c, cell);

			// if (cell.is_attacked) {
			// 	DrawAttack(r, c, cell.owner);
			// }
		}
	}

	// for (auto enemy : model_.getEnemies()) {
	// 	DrawEnemy(enemy);
	// }

	// for (auto unit : model_.getUnits()) {
	// 	DrawUnit(unit);
	// }

	window.display();
}

bool Gui::PollEvent(sf::Event& event) {
	return window.pollEvent(event);
}

Direction Gui::GetLastDirection() const {
	return last_direction_;
}

} // namespace evil
