#include "Strategy.h"
#include "Fragment.h"
#include "Pos.h"
#include "Shapes.h"
#include "Utils.h"

namespace evil {
namespace {

Pos reorient(const Pos& pos, const Alignment& align) {
	assert(isVertical(align.axis0));
	assert(isHorizontal(align.axis1));

	auto max_row = 79;
	auto max_col = 99;

	return {
		align.axis0 == Direction::kDown ? pos.row : max_row - pos.row,
		align.axis1 == Direction::kRight ? pos.col : max_col - pos.col
	};
}

} // namespace


FragmentPtr makeZorroSlice(const Alignment& align) {
	auto vd = align.axis0;
	auto hd = align.axis1;
	auto vdx = opposite(vd);
	auto hdx = opposite(hd);

	auto spike_start = reorient(Pos{1, 21}, align);
	auto diag_start = reorient(Pos{75, 21}, align);

	auto go_origin = std::make_unique<Converge>(spike_start);
	auto do_spike = std::make_unique<SafeRouter>(
		makeSafeSpike({hdx, vd}, 77));
	auto go_diag = std::make_unique<Converge>(diag_start);
	auto do_diag = std::make_unique<SafeRouter>(
		concat(std::vector<Direction>(1, hd), makeDiagonal2({hd, vdx}, 75)));

	auto seq = std::make_unique<Sequence>();
	seq->add(std::move(go_origin));
	seq->add(std::move(do_spike));
	seq->add(std::move(go_diag));
	seq->add(std::move(do_diag));
	return std::move(seq);
}

FragmentPtr makeZorroFinishInside(const Alignment& align) {
	auto vd = align.axis0;
	auto hd = align.axis1;
	auto vdx = opposite(vd);
	auto hdx = opposite(hd);

	auto start1 = reorient(Pos{71, 21}, align);
	auto align1 = Alignment{hd, vdx};
	auto f_start1 = std::make_unique<Converge>(start1);
	auto f_diag1 = std::make_unique<Router>(makeDiagonal2(align1, 70));

	auto start2 = reorient(Pos{1, 91}, align);
	auto align2 = Alignment{vd, hdx};
	auto f_start2 = std::make_unique<Converge>(start2);
	auto f_diag2 = std::make_unique<Router>(makeDiagonal2(align2, 70));

	auto cond = [start1, start2](const Model& model) -> bool {
		auto& unit = model.getUnit(0);
		auto dst_matrix = distanceFill(unit.pos, model.size(),
			[&model](const Pos& p) -> bool {
				auto& cell = model.getCell(p);
				return cell.owner == 1;
			});
		return (dst_matrix.at(start1) <= dst_matrix.at(start2));
	};

	auto seq = std::make_unique<Sequence>();
	auto seq1 = std::make_unique<Sequence>();
	auto seq2 = std::make_unique<Sequence>();

	seq1->add(std::move(f_start1));
	seq1->add(std::move(f_diag1));

	seq2->add(std::move(f_start2));
	seq2->add(std::move(f_diag2));

	auto f_if = std::make_unique<If>(cond, std::move(seq1), std::move(seq2));
	seq->add(std::move(f_if));
	return std::move(seq);
}

FragmentPtr makeZorroFinishOutside(const Alignment& align) {
	auto vd = align.axis0;
	auto hd = align.axis1;
	auto vdx = opposite(vd);
	auto hdx = opposite(hd);

	auto start1 = reorient(Pos{78, 25}, align);
	auto align1 = Alignment{vdx, hd};
	auto f_start1 = std::make_unique<Converge>(start1);
	auto f_diag1 = std::make_unique<Router>(makeDiagonal2(align1, 73));

	auto start2 = reorient(Pos{5, 98}, align);
	auto align2 = Alignment{hdx, vd};
	auto f_start2 = std::make_unique<Converge>(start2);
	auto f_diag2 = std::make_unique<Router>(makeDiagonal2(align2, 73));

	auto cond = [start1, start2](const Model& model) -> bool {
		auto& unit = model.getUnit(0);
		auto dst_matrix = distanceFill(unit.pos, model.size(),
			[&model](const Pos& p) -> bool {
				auto& cell = model.getCell(p);
				return cell.owner == 1;
			});
		return (dst_matrix.at(start1) <= dst_matrix.at(start2));
	};

	auto seq = std::make_unique<Sequence>();
	auto seq1 = std::make_unique<Sequence>();
	auto seq2 = std::make_unique<Sequence>();

	seq1->add(std::move(f_start1));
	seq1->add(std::move(f_diag1));

	seq2->add(std::move(f_start2));
	seq2->add(std::move(f_diag2));

	auto f_if = std::make_unique<If>(cond, std::move(seq1), std::move(seq2));
	seq->add(std::move(f_if));
	return std::move(seq);
}

bool isDangerousInside(const Enemy& enemy, const Alignment& align, const Model& model) {
	auto vd = align.axis0;
	auto hd = align.axis1;
	auto vdx = opposite(vd);
	auto hdx = opposite(hd);

	auto p0 = reorient(Pos{76, 21}, align);

	if (relativeHorizontal(p0, enemy.pos) != hd) {
		return false;
	}

	auto delta_h = std::abs(enemy.pos.col - p0.col);
	auto p1 = neighbor(neighbor(p0, hd, delta_h), vdx, delta_h);

	if (relativeVertical(p1, enemy.pos) != vdx) {
		return false;
	}

	auto delta_v = std::abs(enemy.pos.row - p1.row);
	if (delta_v > 3) {
		return true;
	}

	return
		((enemy.v_dir == vd && enemy.h_dir == hd) ||
		(enemy.v_dir == vdx && enemy.h_dir == hdx));
}

bool canZorroFinishInside(const Alignment& align, const Model& model) {
	for (auto& enemy : model.getInsideEnemies()) {
		if (isDangerousInside(enemy, align, model)) {
			return false;
		}
	}

	return true;
}

bool isDangerousOutside(const Enemy& enemy, const Alignment& align, const Model& model) {
	auto vd = align.axis0;
	auto hd = align.axis1;
	auto vdx = opposite(vd);
	auto hdx = opposite(hd);

	auto p0 = reorient(Pos{76, 21}, align);

	if (relativeHorizontal(p0, enemy.pos) != hd) {
		return false;
	}

	auto delta_h = std::abs(enemy.pos.col - p0.col);
	auto p1 = neighbor(neighbor(p0, hd, delta_h), vdx, delta_h);

	if (relativeVertical(p1, enemy.pos) != vd) {
		return false;
	}

	auto delta_v = std::abs(enemy.pos.row - p1.row);
	if (delta_v > 4) {
		return true;
	}

	return
		((enemy.v_dir == vd && enemy.h_dir == hd) ||
		(enemy.v_dir == vdx && enemy.h_dir == hdx));
}

bool canZorroFinishOutside(const Alignment& align, const Model& model) {
	for (auto& enemy : model.getInsideEnemies()) {
		if (isDangerousOutside(enemy, align, model)) {
			return false;
		}
	}

	return true;
}

bool isZorroFinishedInside(const Alignment& align, const Model& model) {
	auto pos = reorient(Pos{21, 41}, align);
	return model.getCell(pos).owner == 1;
}

bool isZorroFinishedOutside(const Alignment& align, const Model& model) {
	auto pos = reorient(Pos{58, 78}, align);
	return model.getCell(pos).owner == 1;
}

#if 0
FragmentPtr makeFullZorro(const Alignment& align) {
	auto cond_inside = [align](const Model& model) {
		return
			!isZorroFinishedInside(align, model) &&
			canZorroFinishInside(align, model);
	};

	auto cond_outside = [align](const Model& model) {
		return
			!isZorroFinishedOutside(align, model) &&
			canZorroFinishOutside(align, model);
	};

	auto lib_both = std::make_unique<LibrateUntil>(
		[cond_inside, cond_outside](const Model& model) {
			return cond_inside(model) || cond_outside(model);
		});

	auto lib_in = std::make_unique<LibrateUntil>(
		[cond_inside](const Model& model) {
			return cond_inside(model);
		});

	auto lib_out = std::make_unique<LibrateUntil>(
		[cond_outside](const Model& model) {
			return cond_outside(model);
		});

	// fragments

	auto finish_in1 = makeZorroFinishInside(align);
	auto finish_out1 = makeZorroFinishOutside(align);
	auto finish_in2 = makeZorroFinishInside(align);
	auto finish_out2 = makeZorroFinishOutside(align);

	auto seq_in = std::make_unique<Sequence>();
	auto seq_out = std::make_unique<Sequence>();

	seq_in->add(std::move(finish_in1));
	seq_in->add(std::move(lib_out));
	seq_in->add(std::move(finish_out2));

	seq_out->add(std::move(finish_out1));
	seq_in->add(std::move(lib_in));
	seq_out->add(std::move(finish_in2));

	auto slice = makeZorroSlice(align, false);
	auto finish = std::make_unique<If>(cond_inside,
		std::move(seq_in), std::move(seq_out));

	auto seq = std::make_unique<Sequence>();
	seq->add(std::move(slice));
	seq->add(std::move(lib_both));
	seq->add(std::move(finish));
	seq->add(std::make_unique<Librate>());

	return std::move(seq);
}

#endif

} // namespace evil