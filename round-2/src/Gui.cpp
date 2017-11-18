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
	sf::Color(255, 248, 248),
};

} // anonymous namespace


Gui::GuiPlayer::GuiPlayer(Gui* gui)
	: gui_(gui)
{}

void Gui::GuiPlayer::update(const Model& model) {
	gui_->onPlayerUpdate(model);
}

bool Gui::GuiPlayer::isReady() const {
	return gui_->isReady();
}

Model::Moves Gui::GuiPlayer::getMoves() {
	return gui_->getMoves();
}


// Gui /////////////////////////////////////////////////////////////////////////

Gui::Gui()
	: player_(this)
{}

void Gui::drawCell(const Pos& pos, sf::Color color) {
	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(cell_w, cell_h));
	rectangle.setFillColor(color);
	rectangle.setPosition(pos.col * cell_w, pos.row * cell_h);
	window_.draw(rectangle);
}

void Gui::drawDot(const Pos& pos, sf::Color color, float scale) {
    sf::CircleShape circ;
    float r = 3.0f * scale;

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

void Gui::setDrawModel(Model model) {
	model_ = std::move(model);
	updateStatus();
}

void Gui::toggleLibrate() {
	if (mode_ != Mode::kLibrate) {
		mode_ = Mode::kLibrate;
		librate_dir_ = dir_;
	} else {
		mode_ = Mode::kNormal;
	}
}

void Gui::handleKeypress(const sf::Event::KeyEvent& ev) {
	switch (ev.code) {
		case sf::Keyboard::Up: dir_ = Direction::kUp; break;
		case sf::Keyboard::Down: dir_ = Direction::kDown; break;
		case sf::Keyboard::Right: dir_ = Direction::kRight; break;
		case sf::Keyboard::Left: dir_ = Direction::kLeft; break;
		case sf::Keyboard::Escape: window_.close(); break;
		case sf::Keyboard::F: delay_ = 0; break;
		case sf::Keyboard::D: delay_ = 100; break;
		case sf::Keyboard::S: delay_ = 200; break;
		case sf::Keyboard::A: delay_ = 1000; break;
		case sf::Keyboard::L: toggleLibrate(); break;
		default: break;
	}
}

void Gui::handleMouseButton(const sf::Event::MouseButtonEvent& ev) {
	auto pos = windowToPos(ev.x, ev.y);
	auto& cell = model_.getCell(pos);
	if (cell.owner == 1) {
		target_pos_ = pos;
		mode_ = Mode::kTracking;
	}
}

bool Gui::update() {
	sf::Event event;
	while (window_.pollEvent(event)) {
		switch (event.type) {
			case sf::Event::Closed:
				window_.close();
				break;
			case sf::Event::KeyPressed:
				handleKeypress(event.key);
				break;
			case sf::Event::MouseMoved: {
				auto& ev = event.mouseMove;
				mouse_pos_ = windowToPos(ev.x, ev.y);
				break;
			}
			case sf::Event::MouseButtonPressed:
				handleMouseButton(event.mouseButton);
				break;
			default:
				break;
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

	for (int r = 0; r < grid.rows(); ++r) {
		for (int c = 0; c < grid.cols(); ++c) {
			Pos pos{r, c};
			auto& cell = grid(r, c);
			drawCell(pos, cell);
			if (cell.proximity <= 0) {
				drawDot(pos, sf::Color::White);
			}
		}
	}

	for (auto& unit : model_.getUnits()) {
		if (unit.pos != unit.next_pos) {
			drawCell(unit.next_pos, sf::Color(255, 220, 5, 150));
		}
		drawDot(neighbor(unit.next_pos, dir_), sf::Color::Red);
	}

	// for (auto pos : makeTrap(mouse_pos_)) {
	// 	drawCell(pos, sf::Color(50, 230, 250, 100));
	// }

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
	// std::cerr << ss.str() << std::endl;
}

void Gui::close() {
	window_.close();
}

Player& Gui::getPlayer() {
	return player_;
}

void Gui::onPlayerUpdate(const Model& model) {
	model_ = model;
	last_update_ = Clock::now();
}

bool Gui::isReady() {
	auto delta = Clock::now() - last_update_;
	return delta > std::chrono::milliseconds(delay_);
}

Model::Moves Gui::getMoves() {
	Model::Moves moves;
	Direction dir = dir_;

	if (mode_ == Mode::kLibrate) {
		dir = librate_dir_;
		librate_dir_ = opposite(librate_dir_);
	} else if (mode_ == Mode::kTracking) {
		auto pos = model_.getUnits().at(0).pos;
		auto closer_dir = model_.directionTowards(pos, target_pos_);

		if (closer_dir == Direction::kNone) {
			for (auto nb_dir : directions(pos, model_.size())) {
				if (model_.getCell(neighbor(pos, nb_dir)).owner == 1) {
					closer_dir = nb_dir;
					break;
				}
			}
			dir_ = closer_dir;
			toggleLibrate();
		}
		dir = closer_dir;
	}

	dir = model_.adjustDirection(0, dir);
	return {{0, dir}};
}

std::vector<Pos> Gui::makeTrap(const Pos& origin) {
	auto vd = Direction::kUp;
	auto hd = rotateCW(vd);

	auto hdx = opposite(hd);
	auto vdx = opposite(vd);

	std::vector<Direction> dirs = {
		vd, vd, vd, vd, hd, hd, hd,
		hdx, hdx, hdx, vdx, vdx, vdx, vdx,
		hd, hd, hd, vd, vd
	};

	auto pos = origin;
	std::vector<Pos> result;
	result.push_back(pos);
	for (auto& dir : dirs) {
		pos = neighbor(pos, dir);
		result.push_back(pos);
	}
	return result;
}

} // namespace evil
