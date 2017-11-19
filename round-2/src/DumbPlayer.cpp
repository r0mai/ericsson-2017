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
				auto dir = model_.directionTowards(unit.pos, cut_.start);
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
				if (unit.pos == cut_.end) {
					state_ = State::kNewCut;
				} else if (cut_.can_go_fast) {
					return {{0, cut_.direction}};
				} else {
					auto tick = cut_.zigzag_tick;
					cut_.zigzag_tick += 1;
					cut_.zigzag_tick %= 3;
					Direction direction = Direction::kNone;
					switch (tick) {
						case 1: direction = opposite(cut_.direction); break;
						case 0:
						case 2: direction = cut_.direction; break;
					}

					Pos next_pos = neighbor(unit.pos, direction);
					// if we're stepping off, check if safe
					if (model_.getCell(next_pos).owner == 0) {
						if (!IsSafe(next_pos)) {
							direction = opposite(cut_.direction);
							cut_.zigzag_tick = 0;
						} else {
							// if safe to step off, we might be able to go fast
							cut_.can_go_fast = CanGoFast(unit);
						}
					}
					return {{0, direction}};
				}
				break;
			}
		}
	}
}

bool DumbPlayer::CanGoFast(const Unit& unit) const {
	const int kMaxLookahead = 80;
	int end_distance = taxicabDistance(unit.pos, cut_.end);
	if (end_distance > kMaxLookahead) {
		return false;
	}

	auto lookahead = model_.lookaheadEnemies(end_distance + 1);

	int d = 1;
	for (Pos p = neighbor(unit.pos, cut_.direction);
		p != cut_.end;
		p = neighbor(p, cut_.direction), ++d)
	{
		int la_cell = lookahead(p.row, p.col);
		if (la_cell == -1) {
			continue;
		}
		if (d-1 < la_cell) {
			return false;
		}
	}

	return true;
}

void DumbPlayer::FindBestCut() {
	cut_ = Cut{};

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
		cut_.start = {center.row, mins.col - 1};
		cut_.end =   {center.row, maxs.col + 1};
		cut_.direction = Direction::kRight;
		cut_.zigzag_tick = 0;
	} else {
		cut_.start = {mins.row - 1, center.col};
		cut_.end =   {maxs.row + 1, center.col};
		cut_.direction = Direction::kDown;
		cut_.zigzag_tick = 0;
	}

	const auto& unit = model_.getUnit(0);
	auto start_distance = taxicabDistance(unit.pos, cut_.start);
	auto end_distance = taxicabDistance(unit.pos, cut_.end);

	if (end_distance < start_distance) {
		std::swap(cut_.start, cut_.end);
		cut_.direction = opposite(cut_.direction);
	}
}

bool DumbPlayer::IsSafe(Pos pos) const {
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
