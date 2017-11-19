#include "DumbPlayer.h"
#include "Utils.h"

namespace evil {

namespace {

int distanceToLongerSideMid(const Pos& a, const AABB& aabb) {
	Pos size = aabb.size();
	Pos center = aabb.center();

	Pos p1, p2;
	if (size.row > size.col) {
		p1 = {center.row, aabb.mins.col - 1};
		p2 = {center.row, aabb.maxs.col + 1};
	} else {
		p1 = {aabb.mins.row - 1, center.col};
		p2 = {aabb.maxs.row + 1, center.col};
	}

	return std::min(taxicabDistance(a, p1), taxicabDistance(a, p2));
}

} // anonymous namespace

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
				FindBestCut(unit.pos);
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

int DumbPlayer::NumberOfEnemiesInArea(const Matrix<bool>& area) const {
	int count = 0;
	for (auto& enemy : model_.getEnemies()) {
		if (area(enemy.pos.row, enemy.pos.col)) {
			++count;
		}
	}
	return count;
}

void DumbPlayer::FindBestCut(const Pos& unit_pos) {
	cut_ = Cut{};

	auto aabb = FindBestArea(unit_pos);
	Pos center = aabb.center();
	Pos size = aabb.size();

	if (size.row > size.col) {
		cut_.start = {center.row, aabb.mins.col - 1};
		cut_.end =   {center.row, aabb.maxs.col + 1};
		cut_.direction = Direction::kRight;
		cut_.zigzag_tick = 0;
	} else {
		cut_.start = {aabb.mins.row - 1, center.col};
		cut_.end =   {aabb.maxs.row + 1, center.col};
		cut_.direction = Direction::kDown;
		cut_.zigzag_tick = 0;
	}

	auto start_distance = taxicabDistance(unit_pos, cut_.start);
	auto end_distance = taxicabDistance(unit_pos, cut_.end);

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

AABB DumbPlayer::FindBestArea(const Pos& unit_pos) const {
	auto& grid = model_.getGrid();
	Matrix<int> areas{grid.rows(), grid.cols()};

	double max_score = 0;
	AABB max_bounding_box;

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

				auto enemy_count = NumberOfEnemiesInArea(area_matrix);
				auto aabb = getBoundingBox(area_matrix);
				auto distance = distanceToLongerSideMid(unit_pos, aabb);
				auto distance_across = std::min(aabb.rows(), aabb.cols());
				double probabilistic_area = distance_across +
					std::pow(2.0, -double(enemy_count + 1)) * aabb.area() / 2.0;
				auto score = probabilistic_area / double(distance + 3*distance_across);

				if (score > max_score) {
					max_score = score;
					max_bounding_box = aabb;
				}
			}
		}
	}

	return max_bounding_box;
}

} // namespace evil
