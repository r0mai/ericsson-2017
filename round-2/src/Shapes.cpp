#include "Shapes.h"

namespace evil {

std::vector<Direction> makeClamp(const Alignment& align, int width) {
	auto vd = align.axis1;
	auto hd = align.axis0;

	assert(width > 0);
	auto vdx = opposite(vd);
	auto hdx = opposite(hd);
	auto k = 2 + width;

	using Dirs = std::vector<Direction>;
	Dirs part1 = {
		hd, hd, hd, vd, vdx, hdx, hdx, hdx,
		vd, vd, vd
	};

	Dirs part2 = {hd, vd}; // repeat `k` times
	Dirs part3 = {
		hd, hd, hd,
		vdx, vdx, vdx, hdx, hd, vd, vd, vd,
		hdx, hdx, hdx,
	};

	Dirs part4 = {vdx, hdx}; // repeat `k` times
	Dirs part5 = {vdx, vdx, vdx};

	Dirs vec;
	std::copy(part1.begin(), part1.end(), std::back_inserter(vec));
	for (int i = 0; i < k; ++i) {
		std::copy(part2.begin(), part2.end(), std::back_inserter(vec));
	}
	std::copy(part3.begin(), part3.end(), std::back_inserter(vec));
	for (int i = 0; i < k; ++i) {
		std::copy(part4.begin(), part4.end(), std::back_inserter(vec));
	}
	std::copy(part5.begin(), part5.end(), std::back_inserter(vec));

	return vec;
}

std::vector<Direction> makeClamp2(const Alignment& align, int) {
	auto vd = align.axis1;
	auto hd = align.axis0;
	auto vdx = opposite(vd);
	auto hdx = opposite(hd);

	return {
		vd, vd, vd, vdx, vdx, vdx,
		hd, hd,
		vd, hd, vd, hd, vd, hd, vd, hd, vd, hd,
		vd, vd, hdx, hdx, hdx,
		hd, hd, hd,
		vdx, vdx, vdx, vdx, vdx, vdx, vdx
	};
}

std::vector<Direction> makeDiagonal(const Alignment& align, int size) {
	auto hd = align.axis0;
	auto vd = align.axis1;

	std::vector<Direction> vec;
	for (int i = 0; i < size; ++i) {
		vec.push_back(vd);
		vec.push_back(hd);
	}
	return vec;
}

int getDiagCol(int row, const Alignment& diag_align, const Pos& origin) {
	auto d = row - origin.row;
	return neighbor(origin, diag_align.axis1, d).col;
}

int getDiagRow(int col, const Alignment& diag_align, const Pos& origin) {
	auto d = col - origin.col;
	return neighbor(origin, diag_align.axis0, d).row;
}

} // namespace evil
