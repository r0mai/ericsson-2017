#include "Direction.h"

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

} // namespace evil
