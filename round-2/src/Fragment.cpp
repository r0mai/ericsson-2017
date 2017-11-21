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

Direction Sequence::getNextInternal(const Model& model) {
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

Direction Sequence::getNext(const Model& model) {
	while (!isFinished()) {
		auto dir = getNextInternal(model);
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

Capture::Capture(const Pos& bounce, const Alignment& align) {
	trigger_ = getCageTrigger(bounce, align);
	auto origin = getCageOrigin(bounce, align);
	auto converge = std::make_unique<Converge>(origin);
	auto prepare = std::make_unique<Router>();
	prepare->add(getCagePrepare(align));

	auto snap_dirs = getCageSnap(align);
	snap_first_ = snap_dirs.front();
	snap_dirs.erase(snap_dirs.begin());
	snap_.add(snap_dirs);

	seq_.add(std::move(converge));
	seq_.add(std::move(prepare));
}

Direction Capture::getNext(const Model& model) {
	if (phase_ == Phase::kPrepare) {
		auto dir = seq_.getNext(model);
		if (dir != Direction::kNone) {
			return dir;
		}
		phase_ = Phase::kWait;
	}

	if (phase_ == Phase::kWait) {
		auto offset = wait_ % 2;
		++wait_;
		if (isTriggered(model)) {
			phase_ = Phase::kSnap;
			if (offset == 0) {
				return snap_first_;
			}
		} else {
			return (offset == 0 ? snap_first_ : opposite(snap_first_));
		}
	}

	if (phase_ == Phase::kSnap) {
		if (!snap_.isFinished()) {
			return snap_.getNext(model);
		}
		phase_ = Phase::kDone;
	}

	return lib_.getNext(model);
}

bool Capture::isTriggered(const Model& model) const {
	bool triggered = false;
	for (auto& enemy : model.getEnemies()) {
		if (enemy.pos == trigger_) {
			triggered = true;
			break;
		}
	}
	return triggered;
}

bool Capture::isFinished() const {
	return false;
}

#if 0
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
#endif

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
	return seq_.getNext(model);
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

// SafeRouter

SafeRouter::SafeRouter(const Model& model, std::vector<Direction> dirs, int unit_idx)
	: unit_idx_(unit_idx)
	, directions_(std::move(dirs))
{
	// find a safe spot around the starting area to retreat to if needed
	auto pos = model.getUnit(unit_idx_).pos;
	last_pos_ = render(pos, directions_).back();

	bool found = false;
	for (auto dir : directions(pos, model.size())) {
		if (model.getCell(neighbor(pos, dir)).owner == 1) {
			directions_.insert(begin(directions_), opposite(dir));
			found = true;
		}
	}

	if (!found) {
		next_direction_idx_ = 0;
		assert(false);
		return;
	}

	if (!lastIsBlue(model)) {
		directions_.push_back(opposite(directions_.back()));
	}
}

Direction SafeRouter::getNext(const Model& model) {
	auto next_direction = directions_[next_direction_idx_];
	auto current_pos = model.getUnit(unit_idx_).pos;
	auto next_pos = neighbor(current_pos, next_direction);

	bool current_on_blue = model.getCell(current_pos).owner == 1;
	bool next_on_blue = model.getCell(next_pos).owner == 1;

	if (can_go_fast_) {
		++next_direction_idx_;
		return next_direction;
	}

	if (next_on_blue) {
		// staying on blue
		++next_direction_idx_;
		return next_direction;
	}

	if (current_on_blue && model.IsSafeToMoveOutAndBack(next_pos)) {
		can_go_fast_ = canGoFast(model);
		++next_direction_idx_;
		return next_direction;
	}

	auto last_direction = directions_[next_direction_idx_ - 1];
	--next_direction_idx_;
	return opposite(last_direction);
}

bool SafeRouter::lastIsBlue(const Model& model) const {
	return model.getCell(last_pos_).owner == 1;
}

bool SafeRouter::canGoFast(const Model& model) const {
	if (!lastIsBlue(model)) {
		return false;
	}
	auto current_pos = model.getUnit(unit_idx_).pos;

	const int kMaxLookahead = 80;
	int end_distance = directions_.size() - next_direction_idx_;
	if (end_distance > kMaxLookahead) {
		return false;
	}

	bool success = false;
	auto lookahead = model.lookaheadEnemies(
		model.GetInsideEnemies(), end_distance + 1, 1000000, &success);
	if (!success) {
		std::cerr << "Too much lookahead" << std::endl;
		return false;
	}

	int d = 0;
	Pos p = neighbor(current_pos, directions_.at(next_direction_idx_ + d));
	while (next_direction_idx_ + d + 1 < directions_.size()) {
		int la_cell = lookahead(p.row, p.col);
		if (la_cell != -1 && d < la_cell) {
			return false;
		}
		++d;
		p = neighbor(p, directions_.at(next_direction_idx_ + d));
	}

	return true;
}

bool SafeRouter::isFinished() const {
	return next_direction_idx_ == directions_.size();
}

} // namespace evil
