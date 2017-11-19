#pragma once
#include <vector>
#include "Pos.h"
#include "Direction.h"


namespace evil {

struct Trap {
	// Traps work by first plotting the directions, and then
	// librating until an enemy shows up at the trigger position.
	// Upon this case, the last direction should be repeated until
	// we get to a safe position.

	// Note: `trigger` is a relative position from origin.

	std::vector<Direction> dirs;
	Pos trigger;
};

Trap makeTrap(Direction axis0, Direction axis1);
std::vector<Pos> renderTrap(const Pos& origin, const Trap& trap);


} // namespace evil
