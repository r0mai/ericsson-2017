#pragma once
#include "Fragment.h"
#include "Direction.h"


namespace evil {

FragmentPtr makeZorroSlice(const Alignment& align);
FragmentPtr makeZorroFinishInside(const Alignment& align);
FragmentPtr makeZorroFinishOutside(const Alignment& align);
bool canZorroFinishInside(const Alignment& align, const Model& model);
bool canZorroFinishOutside(const Alignment& align, const Model& model);
bool isZorroFinishedInside(const Alignment& align, const Model& model);
bool isZorroFinishedOutside(const Alignment& align, const Model& model);

bool isDangerousInside(const Enemy& enemy, const Alignment& align, const Model& model);
bool isDangerousOutside(const Enemy& enemy, const Alignment& align, const Model& model);

} // namespace evil
