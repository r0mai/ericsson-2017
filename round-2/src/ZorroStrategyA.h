#pragma once

#include "ZorroStrategy.h"

namespace evil {

class ZorroStrategyA : public ZorroStrategy {
public:
	std::unique_ptr<Fragment> createInitFragment(const Model& model) override;
	std::unique_ptr<Fragment> createConquerLeftFragment(const Model& model) override;
	std::unique_ptr<Fragment> createConquerRightFragment(const Model& model) override;
	bool isLeftConquered(const Model& model) override;
	bool isRightConquered(const Model& model) override;
	bool isDangerousOnLeft(const Enemy& enemy, const Model& model) override;
	bool isDangerousOnRight(const Enemy& enemy, const Model& model) override;

};

}
