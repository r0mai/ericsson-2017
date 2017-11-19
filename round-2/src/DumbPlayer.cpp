#include "DumbPlayer.h"
#include "Utils.h"

namespace evil {

void DumbPlayer::update(const Model& model) {
	if (model_.getLevel() != model.getLevel()) {
		state_ = State::kNewMap;
	}
	model_ = model;
}

bool DumbPlayer::isReady() const {
	return true;
}

Model::Moves DumbPlayer::getMoves() {
	const auto& unit = model_.getUnit(0);
	while (true) {
		switch (state_) {
			case State::kNewCut: {
				FindBestCut();
				state_ = State::kGoTowardsCutStart;
				break;
			}
			case State::kGoTowardsCutStart: {
				auto dir = model_.directionTowards(unit.pos, cut_start_);
				if (dir == Direction::kNone) {
					state_ = State::kCut;
				} else {
					return {{0, dir}};
				}
				break;
			}
			case State::kNewMap:
				state_ = State::kNewCut;
				break;
			case State::kCut: {
				if (unit.pos == cut_end_) {
					state_ = State::kNewCut;
				} else {
					auto tick = cut_zigzag_tick_;
					cut_zigzag_tick_ += 1;
					cut_zigzag_tick_ %= 3;
					Direction direction = Direction::kNone;
					switch (tick) {
						case 1: direction = opposite(cut_direction_); break;
						case 0:
						case 2: direction = cut_direction_; break;
					}

					Pos next_pos = neighbor(unit.pos, direction);
					// if we're stepping off, check if safe
					if (model_.getCell(next_pos).owner == 0 && !CheckIfSafe(next_pos)) {
						direction = opposite(cut_direction_);
						cut_zigzag_tick_ = 0;
					}
					return {{0, direction}};
				}
				break;
			}
		}
	}
}

void DumbPlayer::FindBestCut() {
	Pos mins;
	Pos maxs;
	std::tie(mins, maxs) = FindBiggestArea();
	Pos center = {
		(mins.row + maxs.row) / 2,
		(mins.col + maxs.col) / 2
	};

	Pos size = {
		maxs.row - mins.row,
		maxs.col - mins.col
	};

	if (size.row > size.col) {
		cut_start_ = {center.row, mins.col - 1};
		cut_end_ =   {center.row, maxs.col + 1};
		cut_direction_ = Direction::kRight;
		cut_zigzag_tick_ = 0;
	} else {
		cut_start_ = {mins.row - 1, center.col};
		cut_end_ =   {maxs.row + 1, center.col};
		cut_direction_ = Direction::kDown;
		cut_zigzag_tick_ = 0;
	}

	const auto& unit = model_.getUnit(0);
	auto start_distance = taxicabDistance(unit.pos, cut_start_);
	auto end_distance = taxicabDistance(unit.pos, cut_end_);

	if (end_distance < start_distance) {
		std::swap(cut_start_, cut_end_);
		cut_direction_ = opposite(cut_direction_);
	}
}

bool DumbPlayer::CheckIfSafe(Pos pos) const {
	auto enemy_states = model_.allPossibleEnemyStates(2);
	for (int i = 1; i <= 2; ++i) {
		for (auto& enemy_state : enemy_states[i]) {
			if (enemy_state.pos == pos) {
				return false;
			}
		}
	}
	return true;
}

std::pair<Pos, Pos> DumbPlayer::FindBiggestArea() const {
	auto& grid = model_.getGrid();
	Matrix<int> areas{grid.rows(), grid.cols()};

	int max_area = 0;
	std::pair<Pos, Pos> max_bounding_box;

	for (int r = 0; r < grid.rows(); ++r) {
		for (int c = 0; c < grid.cols(); ++c) {
			if (grid(r, c).owner == 0 && areas(r, c) == 0) {
				auto area_matrix = floodFill(grid, {r, c}, [&](const Pos& p) {
					return grid(p.row, p.col).owner == 0;
				});

				mergeMatrices(areas, area_matrix, [&](int& area_idx, bool in_area) {
					if (in_area) {
						area_idx = 1;
					}
				});

				int area = getArea(area_matrix);
				if (area > max_area) {
					max_area = area;
					max_bounding_box = getBoundingBox(area_matrix);
				}
			}
		}
	}

	return max_bounding_box;
}

} // namespace evil
