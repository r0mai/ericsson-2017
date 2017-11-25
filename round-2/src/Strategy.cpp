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
	seq->add(std::make_unique<Librate>()); // todo - collision here
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
	seq->add(std::make_unique<Librate>());
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
	seq->add(std::make_unique<Librate>());
	return std::move(seq);
}


} // namespace evil