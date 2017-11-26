#include "ZorroStrategyA.h"

#include "Fragment.h"
#include "Shapes.h"

namespace evil {

std::unique_ptr<Fragment> ZorroStrategyA::createInitFragment(const Model& model) {
	auto seq = std::make_unique<Sequence>();
	auto go_origin = std::make_unique<Converge>(Pos{27, 1});

	auto small_diag_align = Alignment{Direction::kUp, Direction::kRight};
	auto small_diag = makeDiagonal(small_diag_align, 20);
	auto small_diag_route = std::make_unique<SafeRouter>(small_diag);

	auto make_spike = std::make_unique<SafeRouter>(
		std::vector<Direction>{Direction::kUp, Direction::kUp, Direction::kUp, Direction::kUp, Direction::kUp,
							   Direction::kUp, Direction::kUp});
	auto go_origin_again = std::make_unique<Converge>(Pos{1, 21});

	auto diag_align = Alignment{Direction::kDown, Direction::kRight};
	auto diag = makeDiagonal(diag_align, 77);
	auto diag_route = std::make_unique<SafeRouter>(diag);

	seq->add(std::move(go_origin));
	seq->add(std::move(small_diag_route));
	seq->add(std::move(make_spike));
	seq->add(std::move(go_origin_again));
	seq->add(std::move(diag_route));

	return std::move(seq);
}

std::unique_ptr<Fragment> ZorroStrategyA::createConquerLeftFragment(const Model& model) {
	auto seq = std::make_unique<Sequence>();

	bool is_down = model.getUnit(0).pos.row > 39;
	if (is_down) {
		auto go_origin = std::make_unique<Converge>(Pos{78, 93});
		auto diag_align = Alignment{Direction::kUp, Direction::kLeft};
		auto diag = makeDiagonal(diag_align, 72);
		auto diag_route = std::make_unique<Router>(diag);

		seq->add(std::move(go_origin));
		seq->add(std::move(diag_route));
	} else {
		auto go_origin = std::make_unique<Converge>(Pos{7, 21});
		auto diag_align = Alignment{Direction::kDown, Direction::kRight};
		auto diag = makeDiagonal(diag_align, 73);
		auto diag_route = std::make_unique<Router>(diag);

		seq->add(std::move(go_origin));
		seq->add(std::move(diag_route));
	}

	return std::move(seq);
}

std::unique_ptr<Fragment> ZorroStrategyA::createConquerRightFragment(const Model& model) {
	auto seq = std::make_unique<Sequence>();

	bool is_down = model.getUnit(0).pos.row > 39;
	if (is_down) {
		auto go_origin = std::make_unique<Converge>(Pos{71, 98});
		auto diag_align = Alignment{Direction::kUp, Direction::kLeft};
		auto diag = makeDiagonal(diag_align, 73);
		auto diag_route = std::make_unique<Router>(diag);

		seq->add(std::move(go_origin));
		seq->add(std::move(diag_route));
	} else {
		auto go_origin = std::make_unique<Converge>(Pos{1, 27});
		auto diag_align = Alignment{Direction::kDown, Direction::kRight};
		auto diag = makeDiagonal(diag_align, 73);
		auto diag_route = std::make_unique<Router>(diag);

		seq->add(std::move(go_origin));
		seq->add(std::move(diag_route));
	}

	return std::move(seq);

}

bool ZorroStrategyA::isLeftConquered(const Model& model) {
	return model.getCell(Pos{40, 40}).owner == 1; // check random cell
}

bool ZorroStrategyA::isRightConquered(const Model& model) {
	return model.getCell(Pos{20, 80}).owner == 1; // check random cell
}

bool ZorroStrategyA::isDangerousOnLeft(const Enemy& enemy, const Model& model) {
	auto& pos = enemy.pos;

	Alignment diag_align{Direction::kDown, Direction::kRight};
	auto main_diag_col = getDiagCol(pos.row, diag_align, Pos{1, 21});
	auto safe_diag_col = getDiagCol(pos.row, diag_align, Pos{1, 17});
	auto small_diag_row = getDiagRow(pos.col, Alignment{Direction::kUp, Direction::kRight}, Pos{27, 1});
	int spike_col = 21;

	Alignment enemy_alignment{enemy.v_dir, enemy.h_dir};
	bool outside_enemy = model.getCell(pos).owner == 1;
	bool other_side_of_the_diag = pos.col > main_diag_col;
	bool captured_in_top_left_shape = pos.row < small_diag_row && pos.col < spike_col;
	bool in_safe_zone = pos.col > safe_diag_col;
	bool direction_is_aligned_with_diag = enemy_alignment == diag_align || opposite(enemy_alignment) == diag_align;

	bool not_dangerous = outside_enemy || other_side_of_the_diag || captured_in_top_left_shape || (in_safe_zone && direction_is_aligned_with_diag);
	return !not_dangerous; // sorry.. :( :P :)..
}

bool ZorroStrategyA::isDangerousOnRight(const Enemy& enemy, const Model& model) {
	auto& pos = enemy.pos;

	Alignment diag_align{Direction::kDown, Direction::kRight};
	auto main_diag_col = getDiagCol(pos.row, diag_align, Pos{1, 22});
	auto safe_diag_col = getDiagCol(pos.row, diag_align, Pos{1, 26});

	Alignment enemy_alignment{enemy.v_dir, enemy.h_dir};
	bool outside_enemy = model.getCell(pos).owner == 1;
	bool other_side_of_the_diag = pos.col < main_diag_col;
	bool in_safe_zone = pos.col < safe_diag_col;
	bool direction_is_aligned_with_diag = (enemy_alignment == diag_align || opposite(enemy_alignment) == diag_align);

	bool not_dangerous = outside_enemy || other_side_of_the_diag || (in_safe_zone && direction_is_aligned_with_diag);
	return !not_dangerous;
}

} // namespace evil
