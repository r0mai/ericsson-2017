#include "Replayer.h"
#include "Direction.h"
#include <sstream>


namespace evil {

namespace {

Direction toDirection(char ch) {
	switch (ch) {
		case '>': return evil::Direction::kRight;
		case '<': return evil::Direction::kLeft;
		case '^': return evil::Direction::kUp;
		case 'v': return evil::Direction::kDown;
		default: break;
	}
	return evil::Direction::kNone;
}

} // namespace

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

void Replayer::load(std::istream& input) {
	std::string line;
	while (std::getline(input, line)) {
		std::stringstream ss(line);
		int level;
		int tick;
		int units;
		Model::Moves moves;

		ss >> level;
		ss >> tick;
		ss >> units;
		for (int i = 0; i < units; ++i) {
			int unit = 0;
			char ch;
			ss >> unit >> ch;
			auto dir = toDirection(ch);
			assert(dir != evil::Direction::kNone);
			moves.push_back({unit, dir});
		}

		addMoves(level, tick, moves);
	}

	std::cerr << "Loaded " << commands_.size() << std::endl;
}

} // namespace evil
