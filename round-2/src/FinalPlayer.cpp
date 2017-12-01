#include "FinalPlayer.h"


namespace evil {

void FinalPlayer::update(const Model& model) {
	auto old_level = model_.getLevel();
	auto old_units = model_.getOurUnits();

	model_ = model;

	if (old_level != model_.getLevel()) {
		onNewMap();
		return;
	}

	for (auto& unit : model_.getOurUnits()) {
		for (auto& old_unit : old_units) {
			if (unit.index != old_unit.index) {
				continue;
			}
			if (old_unit.health != unit.health) {
				std::cerr << "FinalPlayer: Unit " << unit.index << " died" << std::endl;
				onUnitDied(unit.index);
			}
		}
	}

	for (auto& pair : fragments_) {
		if (pair.second->isFinished()) {
			onUnitDied(pair.first);
		}
	}
}

void FinalPlayer::onNewMap() {
	std::cerr << "FinalPlayer: onNewMap" << std::endl;
	fragments_.clear();

	auto our_units = model_.getOurUnits();
	for (int i = 0; i < our_units.size(); ++i) {
		auto& unit = our_units[i];
		onUnitDied(unit.index);
	}
}

void FinalPlayer::onUnitDied(int unit_idx) {
	// TODO
	std::cerr << "Restarting " << unit_idx << std::endl;

	auto seq = std::make_unique<Sequence>();
	auto corner = model_.getRandomPos();

	auto& unit = model_.getUnit(unit_idx);
	seq->add(std::make_unique<Converge>(unit.index, corner));

	auto dirs = getPathForAABB(getTraversableAABB(corner, {}));

	seq->add(std::make_unique<SafeRouter>(dirs, unit.index));
	//
	fragments_[unit.index] = std::move(seq);
}

FinalPlayer::TraversableAABB FinalPlayer::getTraversableAABB(
	const Pos& pos, const AABB& other_aabb)
{
	int width = 5;
	int height = 5;
	auto colors = model_.getColorizedGrid((width + height) * 2 + 1);

	auto ur = pos.row;
	auto uc = pos.col;

	std::vector<AABB> aabbs;
	aabbs.push_back({{ur, uc}, {ur + height, uc + width}});
	aabbs.push_back({{ur, uc}, {ur - height, uc - width}});
	aabbs.push_back({{ur, uc}, {ur + height, uc - width}});
	aabbs.push_back({{ur, uc}, {ur - height, uc + width}});

	static int k = 0;

	int best_win = -2;
	AABB best_aabb;
	for (auto& aabb : aabbs) {
		if (hasOverlap(aabb, other_aabb)) {
			continue;
		}
		int win = 0;
		if (win > best_win) {
			best_win = win;
			best_aabb = aabb;
		}
	}

	if (best_win == 0) {
		best_aabb = aabbs[k++ % 4];
	}

	best_aabb.mins.row = std::min(std::max(best_aabb.mins.row, 0), model_.size().row - 1);
	best_aabb.mins.col = std::min(std::max(best_aabb.mins.col, 0), model_.size().col - 1);
	best_aabb.maxs.row = std::min(std::max(best_aabb.maxs.row, 0), model_.size().row - 1);
	best_aabb.maxs.col = std::min(std::max(best_aabb.maxs.col, 0), model_.size().col - 1);

	TraversableAABB result_aabb;
	result_aabb.aabb = best_aabb;
	result_aabb.initial_pos = pos;
	return result_aabb;
}

std::vector<Direction> FinalPlayer::getPathForAABB(const TraversableAABB& aabb) {
	std::vector<Direction> path;
	Direction v_dir, h_dir;
	if (aabb.initial_pos.col < aabb.aabb.maxs.col) {
		h_dir = Direction::kRight;
	} else {
		h_dir = Direction::kLeft;
	}
	if (aabb.initial_pos.row < aabb.aabb.maxs.row) {
		v_dir = Direction::kDown;
	} else {
		v_dir = Direction::kUp;
	}

	for (int i = 0; i < aabb.aabb.size().col; ++i) {
		path.push_back(h_dir);
	}
	for (int i = 0; i < aabb.aabb.size().row; ++i) {
		path.push_back(v_dir);
	}
	for (int i = 0; i < aabb.aabb.size().col; ++i) {
		path.push_back(opposite(h_dir));
	}
	for (int i = 0; i < aabb.aabb.size().row; ++i) {
		path.push_back(opposite(v_dir));
	}

	return path;
}

bool FinalPlayer::isReady() const {
	return true;
}

Model::Moves FinalPlayer::getMoves() {
	Model::Moves moves;

	for (auto& pair : fragments_) {
		auto index = pair.first;
		auto& fragment = pair.second;
		auto dir = fragment->getNext(model_);
		moves.push_back({index, dir});
	}

	return moves;
}

} // namespace evil
