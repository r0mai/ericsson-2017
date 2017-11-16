#include "gui.h"

#include <iostream>

namespace evil {

namespace {

sf::Color owner_colors[] = {sf::Color::White, sf::Color::Blue, sf::Color::Green, sf::Color::Red};

} // anonymous namespace

void Gui::DrawCell(int row_idx, int coll_idx, sf::Color color) {
	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(cell_w, cell_h));
	rectangle.setFillColor(color);
	rectangle.setPosition(coll_idx * cell_w, row_idx * cell_h);

	window.draw(rectangle);
}

void Gui::DrawCell(int row_idx, int coll_idx, int owner) {
	DrawCell(row_idx, coll_idx, owner_colors[owner]);
}

void Gui::DrawAttack(int row_idx, int coll_idx, int owner) {
	auto color = owner_colors[2];
	color.a = 100;
	DrawCell(row_idx, coll_idx, color);
}

void Gui::DrawEnemy(const Enemy& enemy) {
	DrawCell(enemy.pos.row, enemy.pos.col, 3);
}

void Gui::DrawUnit(const Unit& unit) {
	DrawCell(unit.pos.row, unit.pos.col, 2);
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
				case sf::Keyboard::Up:

					break;
			}
		}

	}

	window.clear(sf::Color::Black);

	auto& grid = model_.getGrid();

	for (int r=0; r < grid.width(); ++r) {
		for (int c=0; c < grid.height(); ++c) {
			DrawCell(r, c, grid.at(r, c));

			// if (cell.getAttack().isUnit()) {
			// 	DrawAttack(r, c, cell.getOwner());
			// }
		}
	}

	for (auto enemy : model_.getEnemies()) {
		DrawEnemy(enemy);
	}

	for (auto unit : model_.getUnits()) {
		DrawUnit(unit);
	}

	window.display();

	return window.isOpen();
}

bool Gui::PollEvent(sf::Event& event) {
	return window.pollEvent(event);
}

} // namespace evil
