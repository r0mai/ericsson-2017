#pragma once
#include <vector>
#include "Pos.h"
#include "Model.h"
#include "Direction.h"
#include <boost/optional.hpp>


namespace evil {

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
