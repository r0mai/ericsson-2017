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

struct Alignment {
	Direction axis0 = Direction::kRight;
	Direction axis1 = Direction::kUp;
};

Direction toDirection(int n);
bool isHorizontal(Direction dir);
bool isVertical(Direction dir);

Pos neighbor(const Pos& pos, Direction dir);
Pos neighbor(const Pos& pos, Direction dir, int repeat);
Alignment opposite(Alignment align);
Direction opposite(Direction dir);
Direction rotateCW(Direction dir);
Direction rotateCCW(Direction dir);
std::vector<Direction> getDirections(const Pos& pos, const Pos& size);
std::vector<Pos> render(const Pos& origin, const std::vector<Direction>& dirs);

std::vector<Direction> reverse_if(std::vector<Direction> vec, bool reverse);
// <h_dir, v_dir>
std::pair<Direction, Direction> rotateCW(Direction h_dir, Direction v_dir);
std::pair<Direction, Direction> rotateCCW(Direction h_dir, Direction v_dir);

} // namespace evil
