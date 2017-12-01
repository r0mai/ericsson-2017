#include "FinalPlayer.h"


namespace evil {

void FinalPlayer::update(const Model& model) {
	auto old_level = model_.getLevel();
	auto old_units = model_.getOurUnits();

	model_ = model;

	if (old_level != model_.getLevel()) {
		onNewMap();
		return;
	}

	for (auto& unit : model_.getOurUnits()) {
		for (auto& old_unit : old_units) {
			if (unit.index != old_unit.index) {
				continue;
			}
			if (old_unit.health != unit.health) {
				onUnitDied(unit.index);
			}
		}
	}
}

void FinalPlayer::onNewMap() {
	fragments_.clear();

	int ear_size = 10;

	auto our_units = model_.getOurUnits();
	for (int i = 0; i < our_units.size(); ++i) {
		auto& unit = our_units[i];
		if (i == 0) {
			auto seq = std::make_unique<Sequence>();
			auto corner = model_.getSafeCorner();
			corner = neighbor(
				neighbor(corner, Direction::kLeft),
				Direction::kUp);

			seq->add(std::make_unique<Converge>(unit.index, corner));
			auto dirs = model_.createEar(corner,
				Direction::kLeft, Direction::kUp,
				ear_size, ear_size);

			seq->add(std::make_unique<SafeRouter>(dirs, unit.index));
			seq->add(std::make_unique<SafeLibrate>(unit.index));
			//
			fragments_.emplace(unit.index, std::move(seq));
		} else {
			auto seq = std::make_unique<Sequence>();
			auto corner = model_.getSafeCorner();
			corner = neighbor(
				neighbor(corner, Direction::kRight, 2),
				Direction::kDown, 2);

			seq->add(std::make_unique<Converge>(unit.index, corner));
			auto dirs = model_.createEar(corner,
				Direction::kRight, Direction::kDown,
				ear_size, ear_size);

			seq->add(std::make_unique<SafeRouter>(dirs, unit.index));
			seq->add(std::make_unique<SafeLibrate>(unit.index));
			//
			fragments_.emplace(unit.index, std::move(seq));
		}
	}
}

void FinalPlayer::onUnitDied(int unit_idx) {
	// TODO
	std::cerr << "FinalPlayer: Unit " << unit_idx << " died" << std::endl;
}

bool FinalPlayer::isReady() const {
	return true;
}

Model::Moves FinalPlayer::getMoves() {
	Model::Moves moves;

	for (auto& pair : fragments_) {
		auto index = pair.first;
		auto& fragment = pair.second;
		auto dir = fragment->getNext(model_);
		moves.push_back({index, dir});
	}

	return moves;
}

} // namespace evil
