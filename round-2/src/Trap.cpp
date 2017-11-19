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

	trap.trigger = neighbor(neighbor({0, 0}, vd, 3), hd, 3);
	return trap;
}

boost::optional<Trap> makeAlignedTrap(
	const Model& model, const Pos& origin, bool mirror)
{
	std::vector<Pos> blues = {{0, 0}, {1, 0}, {2, 0}};
	std::vector<Pos> whites = {{1, 1}, {2, 2}, {2, 3}, {3, 3}, {4, 4}};

	std::array<Direction, 4> directions = {
		Direction::kRight,
		Direction::kDown,
		Direction::kLeft,
		Direction::kUp
	};

	for (auto axis0 : directions) {
		auto base_axis = (mirror ? opposite(axis0) : axis0);
		std::array<Direction, 2> normals = {
			rotateCW(base_axis), rotateCCW(base_axis)
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
				return makeTrap(axis0, axis1);
			}
		}
	}

	return boost::none;
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

Pos renderTrigger(const Pos& origin, const Trap& trap) {
	Pos pos = origin;
	pos.row += trap.trigger.row;
	pos.col += trap.trigger.col;
	return pos;
}



} // namespace evil
