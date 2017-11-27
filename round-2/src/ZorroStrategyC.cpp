#include "Strategy.h"
#include "ZorroStrategyC.h"
#include "Fragment.h"


namespace evil {

ZorroStrategyC::ZorroStrategyC(bool is_reverse)
	: is_reverse_(is_reverse)
{}

FragmentPtr ZorroStrategyC::createInitFragment(const Model& model) {
	return makeZorroSlice(getAlign(model));
}

FragmentPtr ZorroStrategyC::createConquerLeftFragment(const Model& model) {
	return makeZorroFinishInside(getAlign(model));
}

FragmentPtr ZorroStrategyC::createConquerRightFragment(const Model& model) {
	return makeZorroFinishOutside(getAlign(model));
}

bool ZorroStrategyC::isLeftConquered(const Model& model) {
	return isZorroFinishedInside(getAlign(model), model);
}

bool ZorroStrategyC::isRightConquered(const Model& model) {
	return isZorroFinishedOutside(getAlign(model), model);
}

bool ZorroStrategyC::isDangerousOnLeft(const Enemy& enemy, const Model& model) {
	return isDangerousInside(enemy, getAlign(model), model);
}

bool ZorroStrategyC::isDangerousOnRight(const Enemy& enemy, const Model& model) {
	return isDangerousOutside(enemy, getAlign(model), model);
}

Alignment ZorroStrategyC::getAlign(const Model& model) const {
	if (is_reverse_) {
		return reverse_align_;
	}
	return default_align_;
}

} // namespace evil
