#pragma once

#include <ostream>

namespace evil {

struct Pos {
	Pos() = default;
	Pos(int row, int col) : row(row), col(col) {}

	int row = 0;
	int col = 0;
};

inline
Pos operator+(const Pos& lhs, const Pos& rhs) {
	return {lhs.row + rhs.row, lhs.col + rhs.col};
}

std::ostream& operator<<(std::ostream& out, const Pos& pos);

} // namespace evil
