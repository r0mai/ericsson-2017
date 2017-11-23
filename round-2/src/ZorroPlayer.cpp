#include "ZorroPlayer.h"

#include "Shapes.h"

namespace evil {

void ZorroPlayer::update(const Model& model) {
	if (model.getLevel() != model_.getLevel()) {
		setWaitingFragment();
		state_ = State::kNewMap;
	}

	model_ = model;
	if (model.getLevel() == 1 && model.getTick() < 180) {
		return;
	}

	if (state_ == State::kWaiting) {
		if (isRightReadyToConquer()) {
			state_ = State::kConquering;
			setConquerRightFragment();
		} else if (isLeftReadyToConquer()) {
			state_ = State::kConquering;
			setConquerLeftFragment();
		}
	} else if (state_ == State::kNewMap) {
		state_ = State::kInitializing;
		setInitFragment();
	}
}

bool ZorroPlayer::isReady() const {
	return true;
}

Model::Moves ZorroPlayer::getMoves() {
	Model::Moves moves;
	auto dir = fragment_->getNext(model_);
	if (fragment_->isFinished()) {
		onFragmentFinished();
	}

	dir = model_.adjustDirection(0, dir);

	return {{0, dir}};
}

bool ZorroPlayer::isRightReadyToConquer() {
	if (model_.getCell(Pos{20, 80}).owner == 1) {
		// already conquered
		return false;
	}

	for (auto& enemy : model_.getEnemies()) {
		auto& pos = enemy.pos;

		Alignment diag_align{Direction::kDown, Direction::kRight};
		auto main_diag_col = getDiagCol(pos.row, diag_align, Pos{1, 22});
		auto safe_diag_col = getDiagCol(pos.row, diag_align, Pos{1, 26});

		Alignment enemy_alignment{enemy.v_dir, enemy.h_dir};
		bool safe = model_.getCell(pos).owner == 1 // outside enemy
			|| pos.col < main_diag_col // other side
			|| (pos.col < safe_diag_col && (enemy_alignment == diag_align || opposite(enemy_alignment) == diag_align));
		if (!safe) {
			return false;
		}
	}
	return true;
}

bool ZorroPlayer::isLeftReadyToConquer() {
	if (model_.getCell(Pos{40, 40}).owner == 1) {
		// already conquered
		return false;
	}

	for (auto& enemy : model_.getEnemies()) {
		auto& pos = enemy.pos;

		Alignment diag_align{Direction::kDown, Direction::kRight};
		auto main_diag_col = getDiagCol(pos.row, diag_align, Pos{1, 21});
		auto safe_diag_col = getDiagCol(pos.row, diag_align, Pos{1, 17});
		auto small_diag_row = getDiagRow(pos.col, Alignment{Direction::kUp, Direction::kRight}, Pos{27, 1});
		int spike_col = 21;

		Alignment enemy_alignment{enemy.v_dir, enemy.h_dir};
		bool safe = model_.getCell(pos).owner == 1 // outside enemy
			|| pos.col > main_diag_col // other side
			|| (pos.row < small_diag_row && pos.col < spike_col) // captured
			|| (pos.col > safe_diag_col && (enemy_alignment == diag_align || opposite(enemy_alignment) == diag_align));
		if (!safe) {
			return false;
		}
	}
	return true;
}

void ZorroPlayer::onFragmentFinished() {
	state_ = State::kWaiting;
	setWaitingFragment();
}

void ZorroPlayer::setFragment(std::unique_ptr<Fragment> fragment) {
	fragment_ = std::move(fragment);
	if (fragment_) {
		fragment_->init(model_);
	}
}

void ZorroPlayer::setInitFragment() {
	setFragment(CreateInitFragment());
}

void ZorroPlayer::setWaitingFragment() {
	setFragment(CreateWaitingFragment());
}

void ZorroPlayer::setConquerLeftFragment() {
	setFragment(CreateConquerLeftFragment(model_));
}

void ZorroPlayer::setConquerRightFragment() {
	setFragment(CreateConquerRightFragment(model_));
}

std::unique_ptr<Fragment> ZorroPlayer::CreateInitFragment() {
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

std::unique_ptr<Fragment> ZorroPlayer::CreateConquerLeftFragment(const Model& model) {
	auto seq = std::make_unique<Sequence>();

	bool is_down = model.getUnit(0).pos.row > 70;
	if (is_down) {
		auto go_origin = std::make_unique<Converge>(Pos{79, 93});
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

std::unique_ptr<Fragment> ZorroPlayer::CreateConquerRightFragment(const Model& model) {
	auto seq = std::make_unique<Sequence>();

	bool is_down = model.getUnit(0).pos.row > 70;
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

std::unique_ptr<Fragment> ZorroPlayer::CreateWaitingFragment() {
	return std::make_unique<Librate>();
}


}