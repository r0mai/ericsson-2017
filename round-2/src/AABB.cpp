#include "AABB.h"

namespace evil {

AABB::AABB(const Pos& mins, const Pos& maxs) : mins(mins), maxs(maxs) {
	this->mins.row = std::min(mins.row, maxs.row);
	this->mins.col = std::min(mins.col, maxs.col);
	this->maxs.row = std::max(mins.row, maxs.row);
	this->maxs.col = std::max(mins.col, maxs.col);
}

Pos AABB::size() const {
	return {
		maxs.row - mins.row,
		maxs.col - mins.col
	};
}

Pos AABB::center() const {
	return {
		(mins.row + maxs.row) / 2,
		(mins.col + maxs.col) / 2
	};
}

int AABB::rows() const {
	return maxs.row - mins.row + 1;
}

int AABB::cols() const {
	return maxs.col - mins.col + 1;
}

int AABB::area() const {
	return rows() * cols();
}

bool AABB::contains(const Pos& pos) const {
	return
		pos.row >= mins.row && pos.row <= maxs.row &&
		pos.col >= mins.col && pos.col <= maxs.col;
}

std::ostream& operator<<(std::ostream& os, const AABB& aabb) {
	os << "(" << aabb.mins << ", " << aabb.maxs << ")";
	return os;
}

} // namespace evil
