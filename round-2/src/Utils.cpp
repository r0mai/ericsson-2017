#include "Utils.h"

#include <cmath>

namespace evil {

int taxicabDistance(const Pos& a, const Pos& b) {
	return std::abs(a.row - b.row) + std::abs(a.col - b.col);
}

int getArea(const Matrix<bool>& matrix) {
	int area = 0;
	for (bool b : matrix) {
		area += b;
	}
	return area;
}

AABB getBoundingBox(const Matrix<bool>& matrix) {
	Pos mins{int(matrix.rows()), int(matrix.cols())};
	Pos maxs{-1, -1};

	for (int r = 0; r < matrix.rows(); ++r) {
		for (int c = 0; c < matrix.cols(); ++c) {
			if (matrix(r, c)) {
				if (r < mins.row) { mins.row = r; }
				if (c < mins.col) { mins.col = c; }
				if (r > mins.row) { maxs.row = r; }
				if (c > mins.col) { maxs.col = c; }
			}
		}
	}

	return {mins, maxs};
}

bool hasOverlap(const AABB& a, const AABB& b) {
	auto x_overlap = std::max(0, std::min(a.maxs.col, b.maxs.col) - std::max(a.mins.col, b.mins.col));
	auto y_overlap = std::max(0, std::min(a.maxs.row, b.maxs.row) - std::max(a.mins.row, b.mins.row));
	auto overlapArea = x_overlap * y_overlap;
	return overlapArea != 0;
}

} // namespace evil
