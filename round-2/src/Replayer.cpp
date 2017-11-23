#include "Replayer.h"


namespace evil {


void Replayer::addMoves(int level, int tick, const Model::Moves& moves) {
	commands_[{level, tick}] = moves;
}

void Replayer::update(const Model& model) {
	model_ = model;
}

bool Replayer::isReady() const {
	return true;
}

Model::Moves Replayer::getMoves() {
	auto level = model_.getLevel();
	auto tick = model_.getTick();
	auto it = commands_.find({level, tick});
	if (it == commands_.end()) {
		return {};
	}

	return it->second;
}

} // namespace evil
