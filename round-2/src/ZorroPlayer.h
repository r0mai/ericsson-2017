#pragma once

#include "Player.h"
#include "Fragment.h"

namespace evil {

class ZorroPlayer : public Player {
public:
	static std::unique_ptr<Fragment> CreateInitFragment();
	static std::unique_ptr<Fragment> CreateConquerLeftFragment(const Model& model);
	static std::unique_ptr<Fragment> CreateConquerRightFragment(const Model& model);
	static std::unique_ptr<Fragment> CreateWaitingFragment();
public:
	void update(const Model& model) override;
	bool isReady() const override;
	Model::Moves getMoves() override;
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
};

}

