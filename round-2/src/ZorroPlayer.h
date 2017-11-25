#pragma once

#include "Player.h"
#include "Fragment.h"
#include "ZorroStrategy.h"
#include "ZorroStrategyManager.h"

namespace evil {

class ZorroPlayer : public Player {
public:
	static std::unique_ptr<Fragment> CreateWaitingFragment();
public:
	void update(const Model& model) override;
	bool isReady() const override;
	Model::Moves getMoves() override;

	void LoadPreviousStrategies(const std::string& filename);
	void SaveStrategies(const std::string& filename);
private:
	enum class State {
		kNewMap,
		kInitializing,
		kWaiting,
		kConquering
	};

	bool isRightReadyToConquer();
	bool isLeftReadyToConquer();

	void onFragmentFinished();
	void setFragment(std::unique_ptr<Fragment> fragment);

	void setWaitingFragment();
	void setInitFragment();
	void setConquerLeftFragment();
	void setConquerRightFragment();

	State state_ = State::kNewMap;
	std::unique_ptr<Fragment> fragment_;
	Model model_;
	ZorroStrategy* strategy_;
	ZorroStrategyManager strategy_manager_;
};

} // namespace
