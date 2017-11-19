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


// Fragmets ////////////////////////////////////////////////////////////////////

// Converge

Converge::Converge(const Pos& target)
	: target_(target)
{}

Direction Converge::getNext(const Model& model) {
	auto& unit = model.getUnit(0);
	auto dir = model.directionTowards(unit.pos, target_);
	is_finished_ = (
		dir == Direction::kNone ||
		neighbor(unit.pos, dir) == target_);
	return dir;
}

bool Converge::isFinished() const {
	return is_finished_;
}

// Librate

Direction Librate::getNext(const Model& model) {
	return opposite(model.getUnit(0).dir);
}

bool Librate::isFinished() const {
	return false;
}

// Router

void Router::add(Direction dir) {
	dirs_.push_back(dir);
}

void Router::add(const std::vector<Direction>& dirs) {
	for (auto dir : dirs) {
		dirs_.push_back(dir);
	}
}

Direction Router::getNext(const Model& model) {
	if (dirs_.empty()) {
		return Direction::kNone;
	}

	auto dir = dirs_.front();
	dirs_.pop_front();
	return dir;
}

bool Router::isFinished() const {
	return dirs_.empty();
}

// Sequence

void Sequence::add(std::unique_ptr<Fragment> fragment) {
	if (!fragment->isFinished()) {
		fragments_.push_back(std::move(fragment));
	}
}

Direction Sequence::getNext(const Model& model) {
	if (fragments_.empty()) {
		return Direction::kNone;
	}

	assert(fragments_.front()->isFinished() == false);
	auto dir = fragments_.front()->getNext(model);

	// clean up
	while (!fragments_.empty() && fragments_.front()->isFinished()) {
		fragments_.pop_front();
	}

	return dir;
}

Direction Sequence::forceGetNext(const Model& model) {
	while (!isFinished()) {
		auto dir = getNext(model);
		if (dir != Direction::kNone) {
			return dir;
		}
	}
	return Direction::kNone;
}

bool Sequence::isFinished() const {
	return fragments_.empty();
}

// Capture

Capture::Capture(const Pos& origin, const Trap& trap)
	: origin_(origin)
	, trap_(trap)
	, status_(Status::kConverge)
{
	trigger1_ = renderTrigger1(origin, trap);
	trigger2_ = renderTrigger2(origin, trap);
	fragment_ = std::make_unique<Converge>(origin);
}

Direction Capture::getNext(const Model& model) {
	auto& unit = model.getUnit(0);

	if (status_ == Status::kConverge && unit.pos == origin_) {
		auto router = std::make_unique<Router>();
		router->add(trap_.prepare);
		fragment_ = std::move(router);
		status_ = Status::kBuild;
	} else if (status_ == Status::kBuild && fragment_->isFinished()) {
		fragment_ = std::make_unique<Librate>();
		status_ = Status::kWait;
		snap_origin_ = unit.pos;
	}

	// Note: librate might be replaced immediately
	if (status_ == Status::kWait && isTriggered(model)) {
		auto router = std::make_unique<Router>();
		if (unit.pos != snap_origin_) {
			auto back_dir = opposite(unit.dir);
			if (neighbor(unit.pos, back_dir) != snap_origin_) {
				assert(false && "Unit was displaced");
			}
			router->add(back_dir);
		}
		router->add(trap_.snap);
		fragment_ = std::move(router);
		status_ = Status::kSnap;
	} else if (status_ == Status::kSnap && fragment_->isFinished()) {
		fragment_ = std::make_unique<Librate>();
		status_ = Status::kDone;
	}

	return fragment_->getNext(model);
}

bool Capture::isTriggered(const Model& model) const {
	bool triggered = false;
	for (auto& enemy : model.getEnemies()) {
		auto enemy_next = neighbor(
			neighbor(enemy.pos, enemy.v_dir), enemy.h_dir);
		if (enemy.pos == trigger1_ && enemy_next == trigger2_) {
			triggered = true;
			break;
		}
	}
	return triggered;
}

bool Capture::isFinished() const {
	return false;
}

// Spike

Spike::Spike(const Pos& origin, Direction dir)
	: origin_(origin)
{
	auto conv = std::make_unique<Converge>(origin);
	auto router = std::make_unique<Router>();

	for (int i = 0; i < 2; ++i) {
		router->add(dir);
	}

	for (int i = 0; i < 2; ++i) {
		router->add(opposite(dir));
	}

	seq_.add(std::move(conv));
	seq_.add(std::move(router));
	seq_.add(std::make_unique<Librate>());
}

Direction Spike::getNext(const Model& model) {
	return seq_.forceGetNext(model);
}

bool Spike::isFinished() const {
	return seq_.isFinished();
}

std::vector<Pos> Spike::render(const Pos& origin, Direction dir) {
	std::vector<Pos> vec;
	vec.push_back(origin);
	vec.push_back(neighbor(origin, dir));
	vec.push_back(neighbor(origin, dir, 2));
	return vec;
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
		case sf::Keyboard::Num2: mode_ = Mode::kTrap; break;
		case sf::Keyboard::Num3: mode_ = Mode::kSpike; break;
		default: break;
	}
}

void Gui::handleMouseButton(const sf::Event::MouseButtonEvent& ev) {
	auto pos = windowToPos(ev.x, ev.y);
	auto& cell = model_.getCell(pos);
	if (cell.owner == 1) {
		if (mode_ == Mode::kNormal) {
			auto seq = std::make_unique<Sequence>();
			seq->add(std::make_unique<Converge>(pos));
			seq->add(std::make_unique<Librate>());
			fragment_ = std::move(seq);
		} else if (mode_ == Mode::kTrap) {
			auto trap = makeAlignedTrap(model_, mouse_pos_, cycle_);
			if (trap) {
				fragment_ = std::make_unique<Capture>(mouse_pos_, *trap);
			}
		} else if (mode_ == Mode::kSpike) {
			auto dir = toDirection(cycle_ % 4);
			fragment_ = std::make_unique<Spike>(mouse_pos_, dir);
		}
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

	if (mode_ == Mode::kTrap) {
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
	} else {
		drawCell(mouse_pos_, sf::Color(50, 230, 250, 100));
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

} // namespace evil
