#include "Fragment.h"

namespace evil {

// Fragmets ////////////////////////////////////////////////////////////////////

// Converge

Converge::Converge(const Pos& target)
	: target_(target)
	, unit_idx_(0)
{}

Converge::Converge(int unit_idx, const Pos& target)
	: target_(target)
	, unit_idx_(unit_idx)
{}

bool Converge::init(const Model& model) {
	auto& unit = model.getUnit(unit_idx_);
	if (unit.pos == target_) {
		is_finished_ = true;
		return false;
	}

	auto dir = model.directionTowards(unit.pos, target_);
	if (dir == Direction::kNone) {
		is_finished_ = true;
		return false;
	}

	return true;
}

Direction Converge::getNext(const Model& model) {
	auto& unit = model.getUnit(unit_idx_);
	if (unit.pos == target_) {
		is_finished_ = true;
		return Direction::kNone;
	}

	auto dir = model.directionTowards(unit.pos, target_);
	assert(dir != Direction::kNone);
	dir = model.SafeBlueMove(unit, dir);

	is_finished_ = neighbor(unit.pos, dir) == target_;
	return dir;
}

bool Converge::isFinished() const {
	return is_finished_;
}

// Librate

bool Librate::init(const Model& model) {
	return true;
}

Direction Librate::getNext(const Model& model) {
	return opposite(model.getUnit(0).dir);
}

bool Librate::isFinished() const {
	return false;
}

// LibrateUntil

LibrateUntil::LibrateUntil(Condition cond)
	: cond_(cond)
{}

bool LibrateUntil::init(const Model& model) {
	if (cond_(model)) {
		is_finished_ = true;
		return false;
	}
	return true;
}

Direction LibrateUntil::getNext(const Model& model) {
	if (cond_(model)) {
		is_finished_ = true;
	}
	return opposite(model.getUnit(0).dir);
}

bool LibrateUntil::isFinished() const {
	return is_finished_;
}

// Router

Router::Router(const std::vector<Direction>& dirs) {
	add(dirs);
}

void Router::add(Direction dir) {
	dirs_.push_back(dir);
}

void Router::add(const std::vector<Direction>& dirs) {
	for (auto dir : dirs) {
		dirs_.push_back(dir);
	}
}

bool Router::init(const Model& model) {
	return !isFinished();
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
	fragments_.push_back(std::move(fragment));
}

bool Sequence::init(const Model& model) {
	return initFirst(model);
}

Direction Sequence::getNext(const Model& model) {
	if (!initFirst(model)) {
		return Direction::kNone;
	}

	auto& first = fragments_[0];
	auto dir = first->getNext(model);

	if (first->isFinished()) {
		initialized_ = false;
		fragments_.pop_front();
	}
	return dir;
}

bool Sequence::initFirst(const Model& model) {
	if (initialized_) {
		return true;
	}

	while (!fragments_.empty()) {
		if (fragments_[0]->init(model)) {
			initialized_ = true;
			break;
		} else {
			fragments_.pop_front();
		}
	}
	return initialized_;
}

bool Sequence::isFinished() const {
	return fragments_.empty();
}


// Capture

Capture::Capture(const Pos& bounce, const Alignment& align) {
	trigger_ = getCageTrigger(bounce, align);
	auto origin = getCageOrigin(bounce, align);
	auto converge = std::make_unique<Converge>(origin);
	auto prepare = std::make_unique<Router>(getCagePrepare(align));
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
			// FIXME: snap and lib are not initialized
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

bool Capture::init(const Model& model) {
	return seq_.init(model);
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


// SafeRouter

SafeRouter::SafeRouter(std::vector<Direction> dirs, int unit_idx)
	: unit_idx_(unit_idx)
	, directions_(std::move(dirs))
{}

bool SafeRouter::init(const Model& model) {
	// find a safe spot around the starting area to retreat to if needed
	auto pos = model.getUnit(unit_idx_).pos;
	last_pos_ = render(pos, directions_).back();

	bool found = false;
	for (auto dir : getDirections(pos, model.size())) {
		if (model.getCell(neighbor(pos, dir)).owner == 1) {
			directions_.insert(begin(directions_), opposite(dir));
			found = true;
			break;
		}
	}

	if (!found) {
		next_direction_idx_ = 0;
		// assert(false);
		return false;
	} else {
		next_direction_idx_ = 1;
	}

	if (!lastIsBlue(model)) {
		directions_.push_back(opposite(directions_.back()));
	}

	return true;
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
		model.getInsideEnemies(), end_distance + 1, 1000000, &success);
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

// If

If::If(Condition cond, FragmentPtr true_frag, FragmentPtr false_frag)
	: cond_(cond)
	, true_(std::move(true_frag))
	, false_(std::move(false_frag))
{}

bool If::init(const Model& model) {
	if (cond_(model)) {
		false_.reset();
	} else {
		true_.reset();
	}

	if (true_) {
		return true_->init(model);
	} else {
		return false_->init(model);
	}
}

bool If::isFinished() const {
	if (true_) {
		return true_->isFinished();
	} else {
		return false_->isFinished();
	}
}

Direction If::getNext(const Model& model) {
	if (true_) {
		return true_->getNext(model);
	} else {
		return false_->getNext(model);
	}
}

// SafeLibrateAround

bool SafeLibrate::init(const Model& model) {
	target_pos_ = model.getUnit(0).pos;
	return true;
}

bool SafeLibrate::isFinished() const {
	return false;
}

Direction SafeLibrate::getNext(const Model& model) {
	auto& unit = model.getUnit(0);
	auto preferred_direction = model.directionTowards(unit.pos, target_pos_);
	auto dir = model.SafeBlueMove(unit, preferred_direction);
	if (dir == Direction::kNone) {
		dir = Direction::kRight;
	}
	return dir;
}

} // namespace evil
