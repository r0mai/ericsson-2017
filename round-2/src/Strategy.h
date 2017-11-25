#pragma once
#include "Fragment.h"
#include "Direction.h"


namespace evil {

FragmentPtr makeZorroSlice(const Alignment& align);
FragmentPtr makeZorroFinishInside(const Alignment& align);
FragmentPtr makeZorroFinishOutside(const Alignment& align);


} // namespace evil
