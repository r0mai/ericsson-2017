#pragma once
#include <vector>
#include "Pos.h"
#include "Model.h"
#include "Direction.h"


namespace evil {

std::vector<Direction> makeClamp(const Alignment& align, int size);
std::vector<Direction> makeClamp2(const Alignment& align, int size);
std::vector<Direction> makeDiagonal(const Alignment& align, int size);

} // namespace evil
