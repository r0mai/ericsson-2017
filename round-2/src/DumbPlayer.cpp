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
				if (cut_.router.isFinished()) {
					state_ = State::kNewCut;
				} else {
					return {{0, cut_.router.getNext(model_)}};
				}
#if 0
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
						if (!model_.IsSafeToMoveOutAndBack(next_pos)) {
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
#endif
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

	bool success = false;
	auto lookahead = model_.lookaheadEnemies(
		GetEnemiesInArea(cut_.aabb), end_distance + 1, 1000000, &success);
	if (!success) {
		std::cerr << "Too much lookahead" << std::endl;
		return false;
	}

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

std::vector<EnemyState> DumbPlayer::GetEnemiesInArea(const AABB& aabb) const {
	std::vector<EnemyState> enemies;
	for (auto& enemy : model_.getEnemies()) {
		if (aabb.contains(enemy.pos)) {
			enemies.push_back(enemy);
		}
	}
	return enemies;
}

void DumbPlayer::FindBestCut(const Pos& unit_pos) {
	cut_ = Cut{};

	auto aabb = FindBestArea(unit_pos);
	Pos center = aabb.center();
	Pos size = aabb.size();

	cut_.aabb = aabb;
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

	std::vector<Direction> router_directions(
		std::abs(taxicabDistance(cut_.start, cut_.end)), cut_.direction);

	cut_.router = SafeRouter{model_, std::move(router_directions)};
}

AABB DumbPlayer::FindBestArea(const Pos& unit_pos) const {
	auto& grid = model_.getGrid();
	Matrix<int> areas_matrix{grid.rows(), grid.cols()};

	struct AreaDesc {
		AABB aabb;
		int area = 0;
		int enemy_count = 0;
		int distance_to_start = 0;
		int distance_across = 0;
	};
	std::vector<AreaDesc> areas;

	for (int r = 0; r < grid.rows(); ++r) {
		for (int c = 0; c < grid.cols(); ++c) {
			if (grid(r, c).owner == 0 && areas_matrix(r, c) == 0) {
				auto area_matrix = floodFill(grid, {r, c}, [&](const Pos& p) {
					return grid(p.row, p.col).owner == 0;
				});

				mergeMatrices(areas_matrix, area_matrix, [&](int& area_idx, bool in_area) {
					if (in_area) {
						area_idx = 1;
					}
				});

				// don't bother with small windows
				auto aabb = getBoundingBox(area_matrix);
				if (aabb.rows() < 8 || aabb.cols() < 8) {
					continue;
				}

				AreaDesc area_desc;
				area_desc.aabb = aabb;
				area_desc.area = aabb.area();
				area_desc.enemy_count = GetEnemiesInArea(aabb).size();
				area_desc.distance_to_start = distanceToLongerSideMid(unit_pos, aabb);
				area_desc.distance_across = std::min(aabb.rows(), aabb.cols());

				areas.push_back(area_desc);
			}
		}
	}

	if (areas.empty()) {
		std::cerr << "Ran out of areas to discover" << std::endl;
		return {};
	}

	int largest_area = std::max_element(begin(areas), end(areas),
		[](const auto& lhs, const auto& rhs) {
			return lhs.aabb.area() < rhs.aabb.area();
		})->aabb.area();

	auto best_area = std::max_element(begin(areas), end(areas),
		[&](const AreaDesc& lhs, const AreaDesc& rhs) {
			double lhs_norm_area = lhs.area / double(largest_area);
			double rhs_norm_area = rhs.area / double(largest_area);

			double lhs_score = (180 - lhs.distance_to_start) * lhs_norm_area;
			double rhs_score = (180 - rhs.distance_to_start) * rhs_norm_area;

			return lhs_score < rhs_score;
		}
	);

	return best_area->aabb;
}

} // namespace evil
