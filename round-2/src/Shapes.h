#pragma once
#include <vector>
#include "Pos.h"
#include "Model.h"
#include "Direction.h"


namespace evil {

std::vector<Direction> makeClamp(const Alignment& align, int size);
std::vector<Direction> makeClamp2(const Alignment& align, int size=0);
std::vector<Direction> makeDiagonal(const Alignment& align, int size);
std::vector<Direction> makeInverseTrap(const Alignment& align);

int getDiagCol(int row, const Alignment& diag_align, const Pos& origin);
int getDiagRow(int col, const Alignment& diag_align, const Pos& origin);


// Spike: axis0 - small parts, axis1 - long part
std::vector<Direction> makeSafeSpike(const Alignment& align, int size);

// Diagonal2: axis0 - first step, axis1 - second step
std::vector<Direction> makeDiagonal2(const Alignment& align, int size);


} // namespace evil
