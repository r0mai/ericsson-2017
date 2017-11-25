#pragma once
#include "ZorroStrategy.h"
#include "Direction.h"


namespace evil {

class ZorroStrategyC : public ZorroStrategy {
public:
	std::unique_ptr<Fragment> createInitFragment(const Model& model) override;
	std::unique_ptr<Fragment> createConquerLeftFragment(const Model& model) override;
	std::unique_ptr<Fragment> createConquerRightFragment(const Model& model) override;
	bool isLeftConquered(const Model& model) override;
	bool isRightConquered(const Model& model) override;
	bool isDangerousOnLeft(const Enemy& enemy, const Model& model) override;
	bool isDangerousOnRight(const Enemy& enemy, const Model& model) override;

private:
	Alignment align_{Direction::kDown, Direction::kRight};
};

} // namespace evil
