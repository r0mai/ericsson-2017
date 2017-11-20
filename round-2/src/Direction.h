#pragma once
#include "Pos.h"
#include <vector>


namespace evil {

enum class Direction {
	kUp,
	kRight,
	kDown,
	kLeft,

	kNone
};

Direction toDirection(int n);

Pos neighbor(const Pos& pos, Direction dir);
Pos neighbor(const Pos& pos, Direction dir, int repeat);
Direction opposite(Direction dir);
Direction rotateCW(Direction dir);
Direction rotateCCW(Direction dir);
std::vector<Direction> directions(const Pos& pos, const Pos& size);
std::vector<Pos> render(const Pos& origin, const std::vector<Direction>& dirs);


} // namespace evil
