#include "FinalPlayer.h"


namespace evil {

void FinalPlayer::update(const Model& model) {
	auto old_level = model_.getLevel();
	model_ = model;

	if (old_level != model_.getLevel()) {
		onNewMap();
		return;
	}
}

void FinalPlayer::onNewMap() {
	fragments_.clear();

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
				5, 5);

			seq->add(std::make_unique<SafeRouter>(dirs, unit.index));
			seq->add(std::make_unique<SafeLibrate>(unit.index));
			//
			fragments_.push_back(std::move(seq));
		} else {
			auto seq = std::make_unique<Sequence>();
			auto corner = model_.getSafeCorner();
			corner = neighbor(
				neighbor(corner, Direction::kRight, 2),
				Direction::kDown, 2);

			seq->add(std::make_unique<Converge>(unit.index, corner));
			auto dirs = model_.createEar(corner,
				Direction::kRight, Direction::kDown,
				5, 5);

			seq->add(std::make_unique<SafeRouter>(dirs, unit.index));
			seq->add(std::make_unique<SafeLibrate>(unit.index));
			//
			fragments_.push_back(std::move(seq));
		}
	}
}

bool FinalPlayer::isReady() const {
	return true;
}

Model::Moves FinalPlayer::getMoves() {
	Model::Moves moves;

	auto our_units = model_.getOurUnits();
	for (int i = 0; i < our_units.size() && i < fragments_.size(); ++i) {
		auto dir = fragments_[i]->getNext(model_);
		auto& unit = our_units[i];
		moves.push_back({our_units[i].index, dir});
	}
	return moves;
}

} // namespace evil
