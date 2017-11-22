#include "Direction.h"
#include <cassert>


namespace evil {

Pos neighbor(const Pos& pos, Direction dir) {
	switch(dir) {
		case Direction::kLeft: return {pos.row, pos.col - 1};
		case Direction::kRight: return {pos.row, pos.col + 1};
		case Direction::kUp: return {pos.row - 1, pos.col};
		case Direction::kDown: return {pos.row + 1, pos.col};
		default: break;
	}

	return pos;
}

Pos neighbor(const Pos& pos, Direction dir, int repeat) {
	switch(dir) {
		case Direction::kLeft: return {pos.row, pos.col - repeat};
		case Direction::kRight: return {pos.row, pos.col + repeat};
		case Direction::kUp: return {pos.row - repeat, pos.col};
		case Direction::kDown: return {pos.row + repeat, pos.col};
		default: break;
	}

	return pos;
}

Direction opposite(Direction dir) {
	switch (dir) {
		case Direction::kLeft: return Direction::kRight;
		case Direction::kRight: return Direction::kLeft;
		case Direction::kUp: return Direction::kDown;
		case Direction::kDown: return Direction::kUp;
		default: return dir;
	}

	return dir;
}

Direction rotateCW(Direction dir) {
	switch (dir) {
		case Direction::kLeft: return Direction::kUp;
		case Direction::kUp: return Direction::kRight;
		case Direction::kRight: return Direction::kDown;
		case Direction::kDown: return Direction::kLeft;
		default: return dir;
	}

	return dir;
}


Direction rotateCCW(Direction dir) {
	switch (dir) {
		case Direction::kLeft: return Direction::kDown;
		case Direction::kUp: return Direction::kLeft;
		case Direction::kRight: return Direction::kUp;
		case Direction::kDown: return Direction::kRight;
		default: return dir;
	}

	return dir;
}

std::vector<Direction> directions(const Pos& pos, const Pos& size) {
	std::vector<Direction> vec;
	if (pos.col > 0) { vec.push_back(Direction::kLeft); }
	if (pos.row > 0) { vec.push_back(Direction::kUp); }
	if (pos.col + 1 < size.col) { vec.push_back(Direction::kRight); }
	if (pos.row + 1 < size.row) { vec.push_back(Direction::kDown); }
	return vec;
}

Direction toDirection(int n) {
	if (n >= 0 && n < 4) {
		return Direction(n);
	}
	assert(false && "Invalid direction index");
	return Direction::kNone;
}

std::vector<Pos> render(const Pos& origin, const std::vector<Direction>& dirs) {
	auto pos = origin;
	std::vector<Pos> vec;

	vec.push_back(pos);
	for (auto dir : dirs) {
		pos = neighbor(pos, dir);
		vec.push_back(pos);
	}
	return vec;
}

std::vector<Direction> reverse_if(std::vector<Direction> vec, bool reverse) {
	if (reverse) {
		std::reverse(vec.begin(), vec.end());
	}
	return vec;
}


} // namespace evil
