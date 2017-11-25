#pragma once

#include <memory>

namespace evil {

class Fragment;
class Model;
class Enemy;

class ZorroStrategy {
public:
	virtual std::unique_ptr<Fragment> createInitFragment(const Model& model) = 0;
	virtual std::unique_ptr<Fragment> createConquerLeftFragment(const Model& model) = 0;
	virtual std::unique_ptr<Fragment> createConquerRightFragment(const Model& model) = 0;

	virtual bool isLeftConquered(const Model& model) = 0;
	virtual bool isRightConquered(const Model& model) = 0;

	virtual bool isDangerousOnLeft(const Enemy& enemy, const Model& model) = 0;
	virtual bool isDangerousOnRight(const Enemy& enemy, const Model& model) = 0;
};

}