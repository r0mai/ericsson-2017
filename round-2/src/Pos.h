#pragma once

#include <ostream>
#include <tuple>

namespace evil {

struct Pos {
	Pos() = default;
	Pos(int row, int col) : row(row), col(col) {}

	int row = 0;
	int col = 0;
};

inline
bool operator<(const Pos& lhs, const Pos& rhs) {
	return
		std::tie(lhs.row, lhs.col) <
		std::tie(rhs.row, rhs.col);
}

inline
bool operator==(const Pos& lhs, const Pos& rhs) {
	return
		std::tie(lhs.row, lhs.col) ==
		std::tie(rhs.row, rhs.col);
}

inline
Pos operator+(const Pos& lhs, const Pos& rhs) {
	return {lhs.row + rhs.row, lhs.col + rhs.col};
}

std::ostream& operator<<(std::ostream& out, const Pos& pos);

} // namespace evil
