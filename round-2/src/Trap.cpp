#include "Trap.h"
#include "Model.h"
#include <cassert>
#include <array>

namespace evil {

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



} // namespace evil
