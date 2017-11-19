#include "Pos.h"

namespace evil {

std::ostream& operator<<(std::ostream& out, const Pos& pos) {
	out << "(" << pos.row << ", " << pos.col << ")";
	return out;
}


} // namespace evil
