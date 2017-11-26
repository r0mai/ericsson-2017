#include "ZorroStrategyB.h"

#include "Fragment.h"
#include "Shapes.h"

namespace evil {

std::unique_ptr<Fragment> ZorroStrategyB::createInitFragment(const Model& model) {
	auto seq = std::make_unique<Sequence>();
	auto go_origin = std::make_unique<Converge>(Pos{1, 21});

	auto make_spike = std::make_unique<SafeRouter>(
		std::vector<Direction>(79, Direction::kDown));

	auto diag_align = Alignment{Direction::kUp, Direction::kRight};
	auto diag = makeDiagonal(diag_align, 77);
	auto diag_route = std::make_unique<SafeRouter>(diag);

	seq->add(std::move(go_origin));
	seq->add(std::move(make_spike));
	seq->add(std::move(diag_route));

	return std::move(seq);
}

std::unique_ptr<Fragment> ZorroStrategyB::createConquerLeftFragment(const Model& model) {
	auto seq = std::make_unique<Sequence>();

	bool is_down = model.getUnit(0).pos.row > 39;
	if (is_down) {
		auto go_origin = std::make_unique<Converge>(Pos{73, 21});
		auto diag_align = Alignment{Direction::kUp, Direction::kRight};
		auto diag = makeDiagonal(diag_align, 75);
		auto diag_route = std::make_unique<Router>(diag);

		seq->add(std::move(go_origin));
		seq->add(std::move(diag_route));
	} else {
		auto go_origin = std::make_unique<Converge>(Pos{1, 94});
		auto diag_align = Alignment{Direction::kDown, Direction::kLeft};
		auto diag = makeDiagonal(diag_align, 73);
		auto diag_route = std::make_unique<Router>(diag);

		seq->add(std::move(go_origin));
		seq->add(std::move(diag_route));
	}

	return std::move(seq);
}

std::unique_ptr<Fragment> ZorroStrategyB::createConquerRightFragment(const Model& model) {
	auto seq = std::make_unique<Sequence>();

	bool is_down = model.getUnit(0).pos.row > 39;
	if (is_down) {
		auto go_origin = std::make_unique<Converge>(Pos{78, 26});
		auto diag_align = Alignment{Direction::kUp, Direction::kRight};
		auto diag = makeDiagonal(diag_align, 76);
		auto diag_route = std::make_unique<Router>(diag);

		seq->add(std::move(go_origin));
		seq->add(std::move(diag_route));
	} else {
		auto go_origin = std::make_unique<Converge>(Pos{7, 98});
		auto diag_align = Alignment{Direction::kDown, Direction::kLeft};
		auto diag = makeDiagonal(diag_align, 76);
		auto diag_route = std::make_unique<Router>(diag);

		seq->add(std::move(go_origin));
		seq->add(std::move(diag_route));
	}

	return std::move(seq);
}

bool ZorroStrategyB::isLeftConquered(const Model& model) {
	return model.getCell(Pos{30, 50}).owner == 1; // check random cell
}

bool ZorroStrategyB::isRightConquered(const Model& model) {
	return model.getCell(Pos{60, 80}).owner == 1; // check random cell
}

bool ZorroStrategyB::isDangerousOnLeft(const Enemy& enemy, const Model& model) {
	auto& pos = enemy.pos;

	Alignment diag_align{Direction::kUp, Direction::kRight};
	auto main_diag_col = getDiagCol(pos.row, diag_align, Pos{78, 21});
	auto safe_diag_col = getDiagCol(pos.row, diag_align, Pos{74, 21});

	Alignment enemy_alignment{enemy.v_dir, enemy.h_dir};
	bool outside_enemy = model.getCell(pos).owner == 1;
	bool other_side_of_the_diag = pos.col > main_diag_col;
	bool captured_in_left_box = pos.col < 21;
	bool in_safe_zone = pos.col > safe_diag_col;
	bool direction_is_aligned_with_diag = (enemy_alignment == diag_align || opposite(enemy_alignment) == diag_align);

	bool not_dangerous = outside_enemy || other_side_of_the_diag || captured_in_left_box || (in_safe_zone && direction_is_aligned_with_diag);
	return !not_dangerous;
}

bool ZorroStrategyB::isDangerousOnRight(const Enemy& enemy, const Model& model) {
	auto& pos = enemy.pos;

	Alignment diag_align{Direction::kUp, Direction::kRight};
	auto main_diag_col = getDiagCol(pos.row, diag_align, Pos{78, 21});
	auto safe_diag_col = getDiagCol(pos.row, diag_align, Pos{78, 26});

	Alignment enemy_alignment{enemy.v_dir, enemy.h_dir};
	bool outside_enemy = model.getCell(pos).owner == 1;
	bool other_side_of_the_diag = pos.col < main_diag_col;
	bool in_safe_zone = pos.col < safe_diag_col;
	bool direction_is_aligned_with_diag = (enemy_alignment == diag_align || opposite(enemy_alignment) == diag_align);

	bool not_dangerous = outside_enemy || other_side_of_the_diag || (in_safe_zone && direction_is_aligned_with_diag);
	return !not_dangerous;
}

} // namespace evil
