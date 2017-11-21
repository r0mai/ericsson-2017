#include "Trap.h"
#include "Model.h"
#include <cassert>
#include <array>
#include <tuple>

namespace evil {

//
// Cage:
// ^  xxxx.
// |  x ...
// |  x .^
// |  xO ^
// |  XXX^
// +--- axis0 -->
//
// X - should be blue
// x - will be blue
// . - incoming direction - should be white
// O - bounce position
// ^ - closing path
//


bool checkCage(
	const Model& model, const Pos& bounce,
	const Alignment& incoming, Alignment* cage_alignment)
{
	if (incoming.axis0 == incoming.axis1 ||
		incoming.axis0 == opposite(incoming.axis1))
	{
		// Note: not sure if this is possible, but better be safe.
		return false;
	}

	// Note: watch out, as these are {row, col} coordinates.
	std::vector<Pos> blues = {{-1, -1}, {-1, 0}, {-1, 1}};
	std::vector<Pos> whites = {{0, 0}, {1, 1}, {2, 1}, {2, 2}, {2, 3}, {3, 3}};

	auto hd = opposite(incoming.axis0);
	auto vd = opposite(incoming.axis1);

	std::vector<Alignment> alignments = {
		{hd, vd},
		{vd, hd}
	};

	for (auto& align : alignments) {
		bool match = true;
		for (auto& rel : blues) {
			auto pos = neighbor(
				neighbor(bounce, align.axis0, rel.col),
				align.axis1, rel.row);
			if (!model.isValid(pos) || model.getCell(pos).owner != 1) {
				match = false;
				break;
			}
		}

		if (!match) {
			continue;
		}

		for (auto& rel : whites) {
			auto pos = neighbor(
				neighbor(bounce, align.axis0, rel.col),
				align.axis1, rel.row);
			if (!model.isValid(pos) || model.getCell(pos).owner != 0) {
				match = false;
				break;
			}
		}

		if (match) {
			if (cage_alignment) {
				*cage_alignment = align;
			}
			return true;
		}
	}

	return false;
}

Pos getCageOrigin(const Pos& bounce, const Alignment& cage_alignment) {
	return neighbor(
		neighbor(bounce, cage_alignment.axis0, -1),
		cage_alignment.axis1, -1);
}

Pos getCageTrigger(const Pos& bounce, const Alignment& cage_alignment) {
	return neighbor(
		neighbor(bounce, cage_alignment.axis0, 2),
		cage_alignment.axis1, 2);
}

std::vector<Direction> getCagePrepare(const Alignment& cage_alignment) {
	auto hd = cage_alignment.axis0;
	auto vd = cage_alignment.axis1;
	auto hdx = opposite(hd);
	auto vdx = opposite(vd);

	return {
		vd, vd, vd, vd, hd, hd, hd,
		hdx, hdx, hdx, vdx, vdx, vdx, vdx,
		hd, hd, hd, vd,
	};
}

std::vector<Direction> getCageSnap(const Alignment& cage_alignment) {
	auto vd = cage_alignment.axis1;
	return { vd, vd, vd };
}

int getCageStepsToBounce() {
	// Note: this might use the model later to check optimal build times.
	return 20;
	// return 21;
}

std::vector<CagePlacement> getCagePlacements(const Model& model) {
	auto& unit = model.getUnit(0);
	if (model.getCell(unit.pos).owner != 1) {
		// Note: we don't handle placements when unit is out-of-blue.
		return {};
	}

	auto dst_matrix = distanceFill(unit.pos, model.size(),
		[&](const Pos& p) -> bool {
			auto& cell = model.getCell(p);
			return cell.owner == 1;
		});

	std::vector<CagePlacement> vec;

	for (auto& enemy : model.getEnemies()) {
		Alignment incoming = {enemy.h_dir, enemy.v_dir};
		int bounce_t = 0;
		Pos bounce;

		std::tie(bounce_t, bounce) = model.getEnemyBounce(enemy);
		if (bounce_t < 0) {
			// Note: this was an outsider.
			continue;
		}

		Alignment cage_alignment;
		if (!checkCage(model, bounce, incoming, &cage_alignment)) {
			continue;
		}

		auto origin = getCageOrigin(bounce, cage_alignment);
		int origin_dst = dst_matrix.at(origin);
		if (origin_dst < 0) {
			// Note: should not happen, but whatever.
			continue;
		}

		if (origin_dst + getCageStepsToBounce() > bounce_t) {
			// Note: would not finish in time.
			continue;
		}

		CagePlacement cage;
		cage.bounce_t = bounce_t;
		cage.bounce = bounce;
		cage.origin_dst = origin_dst;
		cage.align = cage_alignment;
		vec.push_back(cage);
	}

	std::sort(vec.begin(), vec.end(),
		[](const CagePlacement& lhs, const CagePlacement& rhs) {
			auto lhs_u = lhs.bounce_t - lhs.origin_dst;
			auto rhs_u = rhs.bounce_t - rhs.origin_dst;
			return
				std::tie(lhs_u, lhs.bounce_t) <
				std::tie(rhs_u, rhs.bounce_t);
		});
	return vec;
}


///

Trap makeTrap(Direction axis0, Direction axis1) {
	assert(axis0 != axis1);
	assert(axis0 != opposite(axis1));
	auto vd = axis0;
	auto hd = axis1;

	auto hdx = opposite(hd);
	auto vdx = opposite(vd);

	Trap trap;
	trap.prepare = {
		vd, vd, vd, vd, hd, hd, hd,
		hdx, hdx, hdx, vdx, vdx, vdx, vdx,
		hd, hd, hd, vd, vd
	};

	trap.snap = {
		vd, vd
	};

	trap.trigger1 = neighbor(neighbor({0, 0}, vd, 3), hd, 3);
	trap.trigger2 = neighbor(neighbor({0, 0}, vd, 2), hd, 2);
	return trap;
}

boost::optional<Trap> makeAlignedTrap(
	const Model& model, const Pos& origin, int cycle)
{
	std::vector<Pos> blues = {{0, 0}, {1, 0}, {2, 0}};
	std::vector<Pos> whites = {{1, 1}, {2, 2}, {2, 3}, {3, 3}, {4, 4}};

	std::array<Direction, 4> directions = {
		Direction::kRight,
		Direction::kDown,
		Direction::kLeft,
		Direction::kUp
	};

	std::vector<std::pair<Direction, Direction>> axes;

	for (auto axis0 : directions) {
		std::array<Direction, 2> normals = {
			rotateCW(axis0), rotateCCW(axis0)
		};

		for (auto axis1 : normals) {
			bool match = true;
			for (auto rel : blues) {
				auto pos = neighbor(
					neighbor(origin, axis0, rel.col), axis1, rel.row);
				if (!model.isValid(pos) || model.getCell(pos).owner != 1) {
					match = false;
					break;
				}
			}

			if (!match) {
				continue;
			}

			for (auto rel : whites) {
				auto pos = neighbor(
					neighbor(origin, axis0, rel.col), axis1, rel.row);
				if (!model.isValid(pos) || model.getCell(pos).owner != 0) {
					match = false;
					break;
				}
			}

			if (match) {
				axes.push_back({axis0, axis1});
			}
		}
	}

	if (axes.empty()) {
		return boost::none;
	}

	auto choice = axes[cycle % axes.size()];
	return makeTrap(choice.first, choice.second);
}

std::vector<Pos> renderTrap(const Pos& origin, const Trap& trap) {
	auto pos = origin;
	std::vector<Pos> vec;
	vec.push_back(pos);
	for (auto dir : trap.prepare) {
		pos = neighbor(pos, dir);
		vec.push_back(pos);
	}
	return vec;
}

Pos renderTrigger1(const Pos& origin, const Trap& trap) {
	Pos pos = origin;
	pos.row += trap.trigger1.row;
	pos.col += trap.trigger1.col;
	return pos;
}

Pos renderTrigger2(const Pos& origin, const Trap& trap) {
	Pos pos = origin;
	pos.row += trap.trigger2.row;
	pos.col += trap.trigger2.col;
	return pos;
}

std::vector<Direction> makeClamp(Direction vd, Direction hd, int width) {
	assert(width > 0);
	auto vdx = opposite(vd);
	auto hdx = opposite(hd);
	auto k = 2 + width;

	using Dirs = std::vector<Direction>;
	Dirs part1 = {
		hd, hd, hd, vd, vdx, hdx, hdx, hdx,
		vd, vd, vd
	};

	Dirs part2 = {hd, vd}; // repeat `k` times
	Dirs part3 = {
		hd, hd, hd,
		vdx, vdx, vdx, hdx, hd, vd, vd, vd,
		hdx, hdx, hdx,
	};

	Dirs part4 = {vdx, hdx}; // repeat `k` times
	Dirs part5 = {vdx, vdx, vdx};

	Dirs vec;
	std::copy(part1.begin(), part1.end(), std::back_inserter(vec));
	for (int i = 0; i < k; ++i) {
		std::copy(part2.begin(), part2.end(), std::back_inserter(vec));
	}
	std::copy(part3.begin(), part3.end(), std::back_inserter(vec));
	for (int i = 0; i < k; ++i) {
		std::copy(part4.begin(), part4.end(), std::back_inserter(vec));
	}
	std::copy(part5.begin(), part5.end(), std::back_inserter(vec));

	return vec;
}

} // namespace evil
