#include "Gui.h"
#include "Shapes.h"
#include "Strategy.h"

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
	if (player_) {
		player_->update(model);
	} else {
		gui_->onPlayerUpdate(model);
	}
}

bool Gui::GuiPlayer::isReady() const {
	return gui_->isReady() && (!player_ || player_->isReady());
}

Model::Moves Gui::GuiPlayer::getMoves() {
	if (player_) {
		return player_->getMoves();
	} else {
		return gui_->getMoves();
	}
}

void Gui::GuiPlayer::setPlayer(std::unique_ptr<Player> player) {
	player_ = std::move(player);
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
	setFragment(std::make_unique<Librate>());
}

void Gui::toggleManual(Direction dir) {
	dir_ = dir;
	fragment_.reset();
}

void Gui::setFragment(std::unique_ptr<Fragment> fragment) {
	fragment_ = std::move(fragment);
	if (fragment_) {
		fragment_->init(model_);
	}
}

void Gui::setPlayer(std::unique_ptr<Player> player) {
	player_.setPlayer(std::move(player));
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
		case sf::Keyboard::LBracket: delay_ = std::min(delay_ + delay_ / 2 + 20, 1000); break;
		case sf::Keyboard::RBracket: delay_ = std::max(delay_ - delay_ / 2 - 20, 0); break;

		case sf::Keyboard::Tab: toggleCycle(); break;
		case sf::Keyboard::Num1: mode_ = Mode::kNormal; break;
		case sf::Keyboard::Num4: mode_ = Mode::kDiagonal; break;
		case sf::Keyboard::Num5: mode_ = Mode::kClamp; break;
		case sf::Keyboard::Num6: mode_ = Mode::kInverse; break;

		case sf::Keyboard::Space: toggleStepping(false); break;
		case sf::Keyboard::Period: toggleStepping(true); break;
		case sf::Keyboard::O: ++clamp_w_; break;
		case sf::Keyboard::P: clamp_w_ = std::max(1, clamp_w_ - 1); break;
		case sf::Keyboard::R: reverse_ = !reverse_; break;
		case sf::Keyboard::H: toggleHardcore(); break;

		case sf::Keyboard::Z: toggleZorroBegin(); break;
		case sf::Keyboard::A: toggleZorroConquerLeft(); break;
		case sf::Keyboard::S: toggleZorroConquerRight(); break;

		case sf::Keyboard::V: toggleZorroBegin2(); break;
		case sf::Keyboard::F: toggleZorroConquerLeft2(); break;
		case sf::Keyboard::G: toggleZorroConquerRight2(); break;

		case sf::Keyboard::C: toggleCapture(); break;
		case sf::Keyboard::F2: setPlayer(nullptr); toggleLibrate(); break;
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
			seq->add(std::make_unique<SafeLibrate>());
			setFragment(std::move(seq));
		} else if (mode_ == Mode::kDiagonal) {
			auto align = getAlignment();
			auto diag = makeDiagonal(align, diag_w_);
			auto router = std::make_unique<SafeRouter>(diag);
			auto seq = std::make_unique<Sequence>();
			seq->add(std::make_unique<Converge>(pos));
			seq->add(std::move(router));
			seq->add(std::make_unique<SafeLibrate>());
			setFragment(std::move(seq));
		} else if (mode_ == Mode::kClamp) {
			auto align = getAlignment();
			auto clamp = reverse_if(makeClamp2(align, clamp_w_), reverse_);
			auto router = std::make_unique<SafeRouter>(clamp);
			auto seq = std::make_unique<Sequence>();
			seq->add(std::make_unique<Converge>(pos));
			seq->add(std::move(router));
			seq->add(std::make_unique<SafeLibrate>());
			setFragment(std::move(seq));
		} else if (mode_ == Mode::kInverse) {
			auto align = getAlignment();
			auto shape = reverse_if(makeInverseTrap(align), reverse_);
			auto router = std::make_unique<SafeRouter>(shape);
			auto seq = std::make_unique<Sequence>();
			seq->add(std::make_unique<Converge>(pos));
			seq->add(std::move(router));
			seq->add(std::make_unique<SafeLibrate>());
			setFragment(std::move(seq));
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

	for (auto& enemy : model_.getEnemies()) {
		drawDirection(enemy.pos, enemy.h_dir, enemy.v_dir);
	}

	if (mode_ == Mode::kDiagonal) {
		auto align = getAlignment();
		for (auto pos : render(mouse_pos_, makeDiagonal(align, diag_w_))) {
			drawCell(pos, sf::Color(50, 230, 250, 100));
		}
	} else if (mode_ == Mode::kClamp) {
		auto align = getAlignment();
		drawCells(
			render(mouse_pos_, reverse_if(makeClamp2(align, clamp_w_), reverse_)),
			sf::Color(50, 230, 250, 100));
	} else if (mode_ == Mode::kInverse) {
		auto align = getAlignment();
		drawCells(
			render(mouse_pos_, reverse_if(makeInverseTrap(align), reverse_)),
			sf::Color(50, 230, 250, 100));
	} else {
		drawCell(mouse_pos_, sf::Color(50, 230, 250, 100));
	}
	window_.display();
}

void Gui::drawDirection(const Pos& pos, Direction h_dir, Direction v_dir) {
	sf::RectangleShape rectangle;
	auto size = sf::Vector2f((cell_w + cell_h) / 3, (cell_w + cell_h) / 8.0);

	rectangle.setSize(size);
	rectangle.setFillColor(sf::Color::White);
	rectangle.setPosition((pos.col + 0.5) * cell_w, (pos.row + 0.5) * cell_h);
	rectangle.setOrigin(0, size.y / 2.0);


	float angle = 0;
	if (h_dir == Direction::kRight && v_dir == Direction::kUp) {
		angle = -45;
	} else if (h_dir == Direction::kRight && v_dir == Direction::kDown) {
		angle = 45;
	} else if (h_dir == Direction::kLeft && v_dir == Direction::kUp) {
		angle = 225;
	} else if (h_dir == Direction::kLeft && v_dir == Direction::kDown) {
		angle = 135;
	} else {
		std::cout << "Invalid enemy direction: v = "
			<< v_dir << ", h = " << h_dir << std::endl;
	}
	rectangle.setRotation(angle);
	window_.draw(rectangle);
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

Alignment Gui::getAlignment() {
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
	setFragment(std::make_unique<Capture>(cc.bounce, cc.align));
}

void Gui::toggleHardcore() {
	auto seq = std::make_unique<Sequence>();
	auto go_origin = std::make_unique<Converge>(Pos{1, 21});
	auto go_down = std::make_unique<Converge>(Pos{77, 98});
	auto clamp_align = Alignment{Direction::kRight, Direction::kUp};
	auto diag_align = Alignment{Direction::kUp, Direction::kLeft};
	auto clamp = makeClamp2(clamp_align);
	auto diag = makeDiagonal(diag_align, 70);
	std::reverse(clamp.begin(), clamp.end());

	auto clamp_route = std::make_unique<SafeRouter>(clamp);
	auto diag_route = std::make_unique<SafeRouter>(diag);

	seq->add(std::move(go_origin));
	seq->add(std::move(clamp_route));
	seq->add(std::move(go_down));
	seq->add(std::move(diag_route));
	seq->add(std::make_unique<SafeLibrate>());
	setFragment(std::move(seq));
}

void Gui::toggleZorroBegin() {
	auto seq = std::make_unique<Sequence>();
	auto go_origin = std::make_unique<Converge>(Pos{27, 1});

	auto small_diag_align = Alignment{Direction::kUp, Direction::kRight};
	auto small_diag = makeDiagonal(small_diag_align, 20);
	auto small_diag_route = std::make_unique<SafeRouter>(small_diag);

	auto make_spike = std::make_unique<SafeRouter>(
		std::vector<Direction>{Direction::kUp, Direction::kUp, Direction::kUp, Direction::kUp, Direction::kUp,
							   Direction::kUp, Direction::kUp});
	auto go_origin_again = std::make_unique<Converge>(Pos{1, 21});

	auto diag_align = Alignment{Direction::kDown, Direction::kRight};
	auto diag = makeDiagonal(diag_align, 77);
	auto diag_route = std::make_unique<SafeRouter>(diag);

	seq->add(std::move(go_origin));
	seq->add(std::move(small_diag_route));
	seq->add(std::move(make_spike));
	seq->add(std::move(go_origin_again));
	seq->add(std::move(diag_route));


	seq->add(std::make_unique<SafeLibrate>());
	setFragment(std::move(seq));
}

void Gui::toggleZorroConquerLeft() {
	auto seq = std::make_unique<Sequence>();

	bool is_down = model_.getUnit(0).pos.row > 70;
	if (is_down) {
		auto go_origin = std::make_unique<Converge>(Pos{79, 93});
		auto diag_align = Alignment{Direction::kUp, Direction::kLeft};
		auto diag = makeDiagonal(diag_align, 72);
		auto diag_route = std::make_unique<Router>(diag);

		seq->add(std::move(go_origin));
		seq->add(std::move(diag_route));
	} else {
		auto go_origin = std::make_unique<Converge>(Pos{7, 21});
		auto diag_align = Alignment{Direction::kDown, Direction::kRight};
		auto diag = makeDiagonal(diag_align, 73);
		auto diag_route = std::make_unique<Router>(diag);

		seq->add(std::move(go_origin));
		seq->add(std::move(diag_route));
	}

	seq->add(std::make_unique<SafeLibrate>());
	setFragment(std::move(seq));
}

void Gui::toggleZorroConquerRight() {
	auto seq = std::make_unique<Sequence>();

	bool is_down = model_.getUnit(0).pos.row > 70;
	if (is_down) {
		auto go_origin = std::make_unique<Converge>(Pos{71, 98});
		auto diag_align = Alignment{Direction::kUp, Direction::kLeft};
		auto diag = makeDiagonal(diag_align, 73);
		auto diag_route = std::make_unique<Router>(diag);

		seq->add(std::move(go_origin));
		seq->add(std::move(diag_route));
	} else {
		auto go_origin = std::make_unique<Converge>(Pos{1, 27});
		auto diag_align = Alignment{Direction::kDown, Direction::kRight};
		auto diag = makeDiagonal(diag_align, 73);
		auto diag_route = std::make_unique<Router>(diag);

		seq->add(std::move(go_origin));
		seq->add(std::move(diag_route));
	}

	seq->add(std::make_unique<SafeLibrate>());
	setFragment(std::move(seq));
}

void Gui::toggleZorroBegin2() {
	auto align = Alignment{Direction::kDown, Direction::kRight};
	setFragment(makeZorroSlice(align));
}

void Gui::toggleZorroConquerLeft2() {
	auto align = Alignment{Direction::kDown, Direction::kRight};
	setFragment(makeZorroFinishInside(align));
}

void Gui::toggleZorroConquerRight2() {
	auto align = Alignment{Direction::kDown, Direction::kRight};
	setFragment(makeZorroFinishOutside(align));
}

} // namespace evil
