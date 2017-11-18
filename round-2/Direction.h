#pragma once

#include "Pos.h"

namespace evil {

enum class Direction {
	kUp,
	kRight,
	kDown,
	kLeft,

	kNone
};

Pos neighbor(const Pos& pos, Direction dir);
Direction opposite(Direction dir);
Direction rotateCW(Direction dir);
Direction rotateCCW(Direction dir);


} // namespace evil
