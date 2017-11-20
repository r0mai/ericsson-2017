#include "Fragment.h"

namespace evil {

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


// Diagonal

Diagonal::Diagonal(const Pos& origin, Direction dir)
	: origin_(origin)
{
	auto conv = std::make_unique<Converge>(origin);
	auto router = std::make_unique<Router>();

	auto vd = dir;
	auto hd = rotateCW(dir);
	auto vdx = opposite(vd);
	auto hdx = opposite(hd);

	std::vector<Direction> ds = {
		vd, vdx, vd,
		hd, hdx, hd };

	for (int i = 0; i < 10; ++i) {
		router->add(ds);
	}

	seq_.add(std::move(conv));
	seq_.add(std::move(router));
	seq_.add(std::make_unique<Librate>());
}

Direction Diagonal::getNext(const Model& model) {
	return seq_.forceGetNext(model);
}

bool Diagonal::isFinished() const {
	return seq_.isFinished();
}

std::vector<Pos> Diagonal::render(const Pos& origin, Direction dir) {
	Direction ds[2] = {dir, rotateCW(dir)};
	std::vector<Pos> vec;
	auto pos = origin;

	vec.push_back(pos);
	for (int i = 0; i < 20; ++i) {
		pos = neighbor(pos, ds[i % 2]);
		vec.push_back(pos);
	}
	return vec;
}

} // namespace evil
