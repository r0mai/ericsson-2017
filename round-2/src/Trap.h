#pragma once
#include <vector>
#include "Pos.h"
#include "Direction.h"


namespace evil {

struct Trap {
	// Traps work by first plotting the `prepare` directions and then
	// librating until an enemy shows up at the trigger position.
	// Upon this case, the `snap` directions are followed from the
	// last `prepare` position.

	// Note: `trigger` is a relative position from origin.

	Pos trigger;
	std::vector<Direction> prepare;
	std::vector<Direction> snap;
};

Trap makeTrap(Direction axis0, Direction axis1);
std::vector<Pos> renderTrap(const Pos& origin, const Trap& trap);
Pos renderTrigger(const Pos& origin, const Trap& trap);

} // namespace evil
