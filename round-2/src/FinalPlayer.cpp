#include "FinalPlayer.h"

namespace evil {

void FinalPlayer::update(const Model& model) {
	model_ = model;
}

bool FinalPlayer::isReady() const {
	return true;
}

Model::Moves FinalPlayer::getMoves() {
	return {};
}

} // namespace evil
