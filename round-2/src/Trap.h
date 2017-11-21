#pragma once
#include <vector>
#include "Pos.h"
#include "Model.h"
#include "Direction.h"
#include <boost/optional.hpp>


namespace evil {


// -- deprecated --
struct Trap {
	// Traps work by first plotting the `prepare` directions and then
	// librating until an enemy shows up at the `trigger1` position,
	// heading to `trigger2`.
	// Upon this case, the `snap` directions are followed from the
	// last `prepare` position.

	// Note: `trigger1` & `trigger2` are relative to origin.

	Pos trigger1;
	Pos trigger2;
	std::vector<Direction> prepare;
	std::vector<Direction> snap;
};

Trap makeTrap(Direction axis0, Direction axis1);
boost::optional<Trap> makeAlignedTrap(const Model& model, const Pos& pos, int cycle);
std::vector<Pos> renderTrap(const Pos& origin, const Trap& trap);
Pos renderTrigger1(const Pos& origin, const Trap& trap);
Pos renderTrigger2(const Pos& origin, const Trap& trap);
// --


std::vector<Direction> makeClamp(Direction vd, Direction hd, int width=1);

bool checkCage(
	const Model& model, const Pos& bounce,
	const Alignment& incoming, Alignment* cage_alignment);
Pos getCageOrigin(const Pos& bounce, const Alignment& cage_alignment);
Pos getCageTrigger(const Pos& bounce, const Alignment& cage_alignment);
std::vector<Direction> getCagePrepare(const Alignment& cage_alignment);
std::vector<Direction> getCageSnap(const Alignment& cage_alignment);
int getCageStepsToBounce();

struct CagePlacement {
	Pos bounce;
	Alignment align;
	int origin_dst = 0;
	int bounce_t = 0;
};

std::vector<CagePlacement> getCagePlacements(const Model& model);


} // namespace evil
