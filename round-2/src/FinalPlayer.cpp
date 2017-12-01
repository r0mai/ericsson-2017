#include "FinalPlayer.h"


namespace evil {

void FinalPlayer::update(const Model& model) {
	model_ = model;
	if (!fragments_.empty()) {
		return;
	}
	auto our_units = model_.getOurUnits();
	for (int i = 0; i < our_units.size(); ++i) {
		auto& unit = our_units[i];
		if (i == 0) {
			auto seq = std::make_unique<Sequence>();
			auto corner = model.getSafeCorner();
			corner = neighbor(
				neighbor(corner, Direction::kLeft),
				Direction::kRight);

			seq->add(std::make_unique<Converge>(unit.index, corner));
			auto dirs = model.createEar(corner,
				Direction::kLeft, Direction::kUp,
				3, 3);

			seq->add(std::make_unique<SafeRouter>(dirs, unit.index));
			seq->add(std::make_unique<SafeRouter>(dirs, unit.index));
			seq->add(std::make_unique<SafeLibrate>(unit.index));
			//
			fragments_.push_back(std::move(seq));
		} else {
			fragments_.push_back(std::make_unique<SafeLibrate>(unit.index));
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
