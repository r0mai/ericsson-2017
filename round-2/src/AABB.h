#pragma once

#include "Pos.h"

namespace evil {

struct AABB {
	AABB() = default;
	AABB(const Pos& mins, const Pos& maxs);
	Pos mins;
	Pos maxs;

	Pos size() const;
	Pos center() const;

	int rows() const;
	int cols() const;

	int area() const;

	// inclusive
	bool contains(const Pos& pos) const;
};

} // namespace evil
