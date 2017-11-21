#include "Gui.h"
#include "Shapes.h"

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


// GuiPlayer

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

	if (cell.color == 0 &&
		(pos.col + pos.row == map_w - 1 ||
		pos.col == map_h - 2 ||
		pos.col == 2 + map_w - map_h - 1))
	{
		drawDot(pos, sf::Color(0xee, 0xee, 0xee));
	}

}

bool Gui::init(bool stepping) {
	is_stepping_ = stepping;

	window_.setVerticalSyncEnabled(true);
	return true;
}

void Gui::setDrawModel(Model model) {
	model_ = std::move(model);
	updateStatus();
}

void Gui::toggleLibrate() {
	fragment_ = std::make_unique<Librate>();
}

void Gui::toggleManual(Direction dir) {
	dir_ = dir;
	fragment_.reset();
}

void Gui::handleKeypress(const sf::Event::KeyEvent& ev) {
	switch (ev.code) {
		case sf::Keyboard::X: fragment_.reset(); break;
		case sf::Keyboard::Escape: window_.close(); break;

		case sf::Keyboard::Up: toggleManual(Direction::kUp); break;
		case sf::Keyboard::Down: toggleManual(Direction::kDown); break;
		case sf::Keyboard::Right: toggleManual(Direction::kRight); break;
		case sf::Keyboard::Left: toggleManual(Direction::kLeft); break;
		case sf::Keyboard::L: toggleLibrate(); break;
		case sf::Keyboard::F: delay_ = 0; break;
		case sf::Keyboard::D: delay_ = 100; break;
		case sf::Keyboard::S: delay_ = 200; break;
		case sf::Keyboard::A: delay_ = 1000; break;

		case sf::Keyboard::Tab: toggleCycle(); break;
		case sf::Keyboard::Num1: mode_ = Mode::kNormal; break;
		case sf::Keyboard::Num2: mode_ = Mode::kCapture; break;
		case sf::Keyboard::Num3: mode_ = Mode::kSpike; break;
		case sf::Keyboard::Num4: mode_ = Mode::kDiagonal; break;
		case sf::Keyboard::Num5: mode_ = Mode::kClamp; break;
		case sf::Keyboard::Space: toggleStepping(false); break;
		case sf::Keyboard::Period: toggleStepping(true); break;
		case sf::Keyboard::O: ++clamp_w_; break;
		case sf::Keyboard::P: clamp_w_ = std::max(1, clamp_w_ - 1); break;

		case sf::Keyboard::C: toggleCapture(); break;
		default: break;
	}
}

void Gui::handleMouseButton(const sf::Event::MouseButtonEvent& ev) {
	auto pos = windowToPos(ev.x, ev.y);
	last_clicked_pos_ = pos;
	auto& cell = model_.getCell(pos);
	if (cell.owner == 1) {
		if (mode_ == Mode::kNormal) {
			auto seq = std::make_unique<Sequence>();
			seq->add(std::make_unique<Converge>(pos));
			seq->add(std::make_unique<Librate>());
			fragment_ = std::move(seq);
		} else if (mode_ == Mode::kCapture) {
			// auto trap = makeAlignedTrap(model_, mouse_pos_, cycle_);
			// if (trap) {
			// 	fragment_ = std::make_unique<Capture>(mouse_pos_, *trap);
			// }
		} else if (mode_ == Mode::kSpike) {
			auto dir = toDirection(cycle_ % 4);
			fragment_ = std::make_unique<Spike>(mouse_pos_, dir);
		} else if (mode_ == Mode::kDiagonal) {
			auto dir = toDirection(cycle_ % 4);
			fragment_ = std::make_unique<Diagonal>(mouse_pos_, dir);
		} else if (mode_ == Mode::kClamp) {
			auto router = std::make_unique<Router>();
			auto seq = std::make_unique<Sequence>();
			auto axes = cycledAxes();
			router->add(makeClamp(axes.first, axes.second, clamp_w_));

			seq->add(std::make_unique<Converge>(pos));
			seq->add(std::move(router));
			seq->add(std::make_unique<Librate>());
			fragment_ = std::move(seq);
		}
	}
}

bool Gui::update() {
	std::this_thread::sleep_for(std::chrono::milliseconds(8));

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

	if (mode_ == Mode::kCapture) {
		auto trap = makeAlignedTrap(model_, mouse_pos_, cycle_);
		if (trap) {
			for (auto pos : renderTrap(mouse_pos_, *trap)) {
				drawCell(pos, sf::Color(50, 230, 250, 100));
			}
		} else {
			trap = makeTrap(Direction::kUp,
				cycle_ % 2 != 0 ? Direction::kLeft : Direction::kRight);
			for (auto pos : renderTrap(mouse_pos_, *trap)) {
				drawCell(pos, sf::Color(180, 180, 180, 80));
			}
		}
	} else if (mode_ == Mode::kSpike) {
		auto dir = toDirection(cycle_ % 4);
		for (auto pos : Spike::render(mouse_pos_, dir)) {
			drawCell(pos, sf::Color(50, 230, 250, 100));
		}
	} else if (mode_ == Mode::kDiagonal) {
		auto dir = toDirection(cycle_ % 4);
		for (auto pos : Diagonal::render(mouse_pos_, dir)) {
			drawCell(pos, sf::Color(50, 230, 250, 100));
		}
	} else if (mode_ == Mode::kClamp) {
		auto axes = cycledAxes();
		drawCells(
			render(mouse_pos_, makeClamp(axes.first, axes.second, clamp_w_)),
			sf::Color(50, 230, 250, 100));
	} else {
		drawCell(mouse_pos_, sf::Color(50, 230, 250, 100));
	}
	window_.display();
}

void Gui::drawCells(const std::vector<Pos>& vec, sf::Color color) {
	for (auto pos : vec) {
		drawCell(pos, color);
	}
}

bool Gui::pollEvent(sf::Event& event) {
	return window_.pollEvent(event);
}

Direction Gui::getDirection() const {
	return dir_;
}

void Gui::updateStatus() {
	std::stringstream ss;
	float coverage = round(model_.getCoverage() / 80.0f);
	ss << "Level " << model_.getLevel();
	ss << " - Tick " << model_.getTick();
	ss << " - Coverage " << coverage << "%";
	window_.setTitle(ss.str());
}

void Gui::close() {
	window_.close();
}

Player& Gui::getPlayer() {
	return player_;
}

Pos Gui::lastClickedPosAndClear() {
	auto tmp = last_clicked_pos_;
	last_clicked_pos_ = {};
	return tmp;
}

void Gui::onPlayerUpdate(const Model& model) {
	model_ = model;
	last_update_ = Clock::now();
	step_ready_ = false;
}

bool Gui::isReady() {
	if (is_stepping_) {
		return step_ready_;
	}

	auto delta = Clock::now() - last_update_;
	return delta > std::chrono::milliseconds(delay_);
}

Model::Moves Gui::getMoves() {
	Model::Moves moves;
	auto dir = dir_;

	if (fragment_) {
		dir = fragment_->getNext(model_);
		if (fragment_->isFinished()) {
			fragment_.reset();
		}
		dir_ = dir;
	}

	dir = model_.adjustDirection(0, dir);

	return {{0, dir}};
}

void Gui::toggleCycle() {
	++cycle_;
}

void Gui::toggleStepping(bool enable) {
	step_ready_ = true;
	is_stepping_ = enable;
}

std::pair<Direction, Direction> Gui::cycledAxes() {
	auto mirror = !!((cycle_ % 8) / 4);
	auto axis0 = toDirection(cycle_ % 4);
	auto axis1 = mirror ? rotateCCW(axis0) : rotateCW(axis0);
	return {axis0, axis1};
}

void Gui::toggleCapture() {
	auto placements = getCagePlacements(model_);
	for (auto& pm : placements) {
		std::cerr << "P " << pm.bounce << " " << pm.bounce_t << " "
			<< pm.origin_dst << std::endl;
	}

	if (placements.empty()) {
		return;
	}

	auto& cc = placements.front();
	fragment_ = std::make_unique<Capture>(cc.bounce, cc.align);
}


} // namespace evil
