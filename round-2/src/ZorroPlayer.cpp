#include "ZorroPlayer.h"

#include <vector>

#include "ZorroStrategyA.h"
#include "ZorroStrategyB.h"

namespace evil {



void ZorroPlayer::update(const Model& model) {
	if (model.getLevel() != model_.getLevel()) {
		setWaitingFragment();
		state_ = State::kNewMap;
	}


	model_ = model;
	if (model_.getTick() < strategy_manager_.getSkipTicksNo(model_.getLevel())) {
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
		strategy_ = strategy_manager_.getStrategy(model_.getLevel());
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
	if (strategy_->isRightConquered(model_)) {
		return false;
	}

	for (auto& enemy : model_.getEnemies()) {
		if (strategy_->isDangerousOnRight(enemy, model_)) {
			return false;
		}
	}
	return true;
}

bool ZorroPlayer::isLeftReadyToConquer() {
	if (strategy_->isLeftConquered(model_)) {
		return false;
	}

	for (auto& enemy : model_.getEnemies()) {
		if (strategy_->isDangerousOnLeft(enemy, model_)) {
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
	setFragment(strategy_->createInitFragment(model_));
}

void ZorroPlayer::setWaitingFragment() {
	setFragment(CreateWaitingFragment());
}

void ZorroPlayer::setConquerLeftFragment() {
	setFragment(strategy_->createConquerLeftFragment(model_));
}

void ZorroPlayer::setConquerRightFragment() {
	setFragment(strategy_->createConquerRightFragment(model_));
}

std::unique_ptr<Fragment> ZorroPlayer::CreateWaitingFragment() {
	return std::make_unique<Librate>();
}

void ZorroPlayer::LoadPreviousStrategies(const std::string& filename) {
	strategy_manager_.load(filename);
}

void ZorroPlayer::SaveStrategies(const std::string& filename) {
	strategy_manager_.save(filename);
}

}