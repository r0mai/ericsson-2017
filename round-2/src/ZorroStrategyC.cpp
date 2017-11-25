#include "Strategy.h"
#include "ZorroStrategyC.h"
#include "Fragment.h"


namespace evil {

FragmentPtr ZorroStrategyC::createInitFragment(const Model& model) {
	return makeZorroSlice(align_);
}

FragmentPtr ZorroStrategyC::createConquerLeftFragment(const Model& model) {
	return makeZorroFinishInside(align_);
}

FragmentPtr ZorroStrategyC::createConquerRightFragment(const Model& model) {
	return makeZorroFinishOutside(align_);
}

bool ZorroStrategyC::isLeftConquered(const Model& model) {
	return isZorroFinishedInside(align_, model);
}

bool ZorroStrategyC::isRightConquered(const Model& model) {
	return isZorroFinishedOutside(align_, model);
}

bool ZorroStrategyC::isDangerousOnLeft(const Enemy& enemy, const Model& model) {
	return isDangerousInside(enemy, align_, model);
}

bool ZorroStrategyC::isDangerousOnRight(const Enemy& enemy, const Model& model) {
	return isDangerousOutside(enemy, align_, model);
}

} // namespace evil
