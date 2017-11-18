#include "DumbPlayer.h"

namespace evil {

void DumbPlayer::update(const Model& model) {
	if (model_.getLevel() != model.getLevel()) {
		state_ = State::kNewMap;
	}
	model_ = model;
}

bool DumbPlayer::isReady() const {
	return true;
}

Model::Moves DumbPlayer::getMoves() const {
	const auto& unit = model_.getUnits()[0];
	switch (state_) {
		case State::kNewMap:
			if (unit.pos.row < 43) {
				return {{0, Direction::kDown}};
			} else {
				return {{0, Direction::kUp}};
			}
			break;
	}
}

} // namespace evil
