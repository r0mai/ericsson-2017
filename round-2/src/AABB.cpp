#include "AABB.h"

namespace evil {

AABB::AABB(const Pos& mins, const Pos& maxs) : mins(mins), maxs(maxs) {}

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
	return maxs.row - mins.row;
}

int AABB::cols() const {
	return maxs.row - mins.row;
}

int AABB::area() const {
	return rows() * cols();
}

} // namespace evil
