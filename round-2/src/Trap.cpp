#include "Trap.h"
#include <cassert>


namespace evil {

Trap makeTrap(Direction axis0, Direction axis1) {
	assert(axis0 != axis1);
	assert(axis0 != opposite(axis1));
	auto vd = axis0;
	auto hd = axis1;

	auto hdx = opposite(hd);
	auto vdx = opposite(vd);

	Trap trap;
	trap.dirs = {
		vd, vd, vd, vd, hd, hd, hd,
		hdx, hdx, hdx, vdx, vdx, vdx, vdx,
		hd, hd, hd, vd, vd
	};

	trap.trigger = neighbor(neighbor({0, 0}, vd, 3), hd, 3);
	return trap;
}

std::vector<Pos> renderTrap(const Pos& origin, const Trap& trap) {
	auto pos = origin;
	std::vector<Pos> vec;
	vec.push_back(pos);
	for (auto dir : trap.dirs) {
		pos = neighbor(pos, dir);
		vec.push_back(pos);
	}
	return vec;
}

} // namespace evil
