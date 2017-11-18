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

Model::Moves DumbPlayer::getMoves() {
	const auto& unit = model_.getUnits()[0];
	while (true) {
		switch (state_) {
			case State::kNewMap:
				{
					auto dir = model_.directionTowards(unit.pos, {40, 0});
					if (dir == Direction::kNone) {
						cut_zigzag_tick_ = 0;
						cut_start_ = unit.pos;
						cut_end_ = {unit.pos.row, model_.size().col - 1};
						cut_direction_ = Direction::kRight;
						state_ = State::kCut;
					} else {
						return {{0, dir}};
					}
				}
				break;
			case State::kCut:
				{
					auto tick = cut_zigzag_tick_;
					cut_zigzag_tick_ += 1;
					cut_zigzag_tick_ %= 3;
					switch (tick) {
						case 2:
						case 0: return {{0, cut_direction_}};
						case 1: return {{0, opposite(cut_direction_)}};
					}
				}
		}
	}
}

} // namespace evil
