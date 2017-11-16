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

void Gui::DrawEnemy(protocol::Enemy::Reader& enemy) {
	DrawCell(enemy.getPosition().getRow(), enemy.getPosition().getCol(), 3);
}

void Gui::DrawUnit(protocol::Unit::Reader& unit) {
	DrawCell(unit.getPosition().getRow(), unit.getPosition().getCol(), 2);
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

void Gui::DrawGameState(protocol::Response::Reader& state) {
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
	for (int row_idx=0; row_idx < state.getCells().size(); ++row_idx) {
		auto row = state.getCells()[row_idx];
		for (int coll_idx = 0; coll_idx < row.size(); ++coll_idx) {
			auto cell = row[coll_idx];
			DrawCell(row_idx, coll_idx, cell.getOwner());
			if (cell.getAttack().isUnit()) {
				DrawAttack(row_idx, coll_idx, cell.getOwner());
			}
		}
	}

	for (auto enemy : state.getEnemies()) {
		DrawEnemy(enemy);
	}

	for (auto unit : state.getUnits()) {
		DrawUnit(unit);
	}
	window.display();
}

bool Gui::PollEvent(sf::Event& event) {
	return window.pollEvent(event);
}

} // namespace evil
