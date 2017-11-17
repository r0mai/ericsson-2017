#include "Model.h"
#include <cassert>


namespace evil {
namespace {

Direction toDirection(int i) {
	assert(i >= 0 && i < 4);
	return Direction(i);
}

} // namespace

Direction fromDirection(protocol::Direction dir) {
	switch(dir) {
		case protocol::Direction::LEFT: return Direction::kLeft;
		case protocol::Direction::RIGHT: return Direction::kRight;
		case protocol::Direction::UP: return Direction::kUp;
		case protocol::Direction::DOWN: return Direction::kDown;
		default: break;
	}

	return Direction::kNone;
}

Direction opposite(Direction dir) {
	switch (dir) {
		case Direction::kLeft: return Direction::kRight;
		case Direction::kRight: return Direction::kLeft;
		case Direction::kUp: return Direction::kDown;
		case Direction::kDown: return Direction::kUp;
		default: return dir;
	}

	return dir;
}

Pos neighbor(const Pos& pos, Direction dir) {
	switch(dir) {
		case Direction::kLeft: return {pos.row, pos.col - 1};
		case Direction::kRight: return {pos.row, pos.col + 1};
		case Direction::kUp: return {pos.row - 1, pos.col};
		case Direction::kDown: return {pos.row + 1, pos.col};
		default: break;
	}

	return pos;
}

std::ostream& operator<<(std::ostream& out, const Pos& pos) {
	out << "(" << pos.row << ", " << pos.col << ")";
	return out;
}

std::ostream& operator<<(std::ostream& out, Direction dir) {
	switch (dir) {
		case Direction::kLeft: out << '<'; break;
		case Direction::kRight: out << '>'; break;
		case Direction::kUp: out << '^'; break;
		case Direction::kDown: out << 'v'; break;
		default: out << '.'; break;
	}
	return out;
}


const Model::Grid& Model::getGrid() const {
	return grid_;
}

const std::vector<Unit>& Model::getUnits() const {
	return units_;
}

const std::vector<Enemy>& Model::getEnemies() const {
	return enemies_;
}

const std::string& Model::getStatus() const {
	return status_;
}

int Model::getTick() const {
	return tick_;
}

int Model::getLevel() const {
	return level_;
}

int Model::getOwns() const {
	return owns_;
}

bool Model::update(protocol::Response::Reader response) {
	auto& mat = grid_;
	int row = 0;

	status_ = response.getStatus().cStr();
	tick_ = response.getInfo().getTick();
	level_ = response.getInfo().getLevel();
	owns_ = response.getInfo().getOwns();

	if (response.getCells().size() == 0) {
		return false;
	}

	for (auto cells : response.getCells()) {
		int col = 0;
		for (auto cell : cells) {
			mat(row, col).owner = cell.getOwner();
			mat(row, col).is_attacked = cell.getAttack().isUnit();
			++col;
		}
		++row;
	}
	units_.clear();
	enemies_.clear();

	for (auto enemy : response.getEnemies()) {
		auto pos = enemy.getPosition();
		Enemy e;
		e.pos = Pos{pos.getRow(), pos.getCol()};
		e.v_dir = fromDirection(enemy.getDirection().getVertical());
		e.h_dir = fromDirection(enemy.getDirection().getHorizontal());
		enemies_.push_back(e);
	}

	for (auto unit : response.getUnits()) {
		auto pos = unit.getPosition();
		Unit u;
		u.pos = Pos{pos.getRow(), pos.getCol()};
		u.dir = fromDirection(unit.getDirection());
		units_.push_back(u);
	}

	colorize();
	return true;
}

void Model::colorize() {
	for (auto& cell : grid_) {
		cell.color = 0;
		if (cell.owner == 1) {
			cell.color = 1;
		}
		if (cell.is_attacked) {
			cell.color = 2;
		}
	}

	for (auto& unit : units_) {
		auto pos = unit.pos;
		getCell(pos).color = 3;
	}

	for (auto& enemy : enemies_) {
		auto pos = enemy.pos;
		auto last_pos = pos;
		while (getCell(pos).owner != 1) {
			getCell(pos).color = 5;
			last_pos = pos;
			pos = neighbor(pos, enemy.v_dir);
			pos = neighbor(pos, enemy.h_dir);
		}

		pos = enemy.pos;
		while (getCell(pos).owner != 1) {
			auto& cc = getCell(pos).color;
			if (cc == 0) { cc = 6; }
			pos = neighbor(pos, opposite(enemy.v_dir));
			pos = neighbor(pos, opposite(enemy.h_dir));
		}

		pos = last_pos;
		while (getCell(pos).owner != 1) {
			auto& cc = getCell(pos).color;
			if (cc == 0) { cc = 6; }
			pos = neighbor(pos, opposite(enemy.v_dir));
			pos = neighbor(pos, enemy.h_dir);
		}

		pos = last_pos;
		while (getCell(pos).owner != 1) {
			auto& cc = getCell(pos).color;
			if (cc == 0) { cc = 6; }
			pos = neighbor(pos, enemy.v_dir);
			pos = neighbor(pos, opposite(enemy.h_dir));
		}
	}

	for (auto& enemy : enemies_) {
		auto pos = enemy.pos;
		getCell(pos).color = 4;
	}
}

Direction Model::adjustDirection(int unit_index, Direction dir) {
	auto& unit = units_[unit_index];

	if (dir == Direction::kNone) {
		for (int i = 0; i < 4; ++i) {
			auto nb_dir = toDirection(i);
			auto nb_pos = neighbor(unit.pos, nb_dir);
			if (isValid(nb_pos)) {
				return nb_dir;
			}
		}

		assert(false && "Could not adjust direction");
		return Direction::kNone;
	}

	auto pos = neighbor(unit.pos, dir);
	if (isValid(pos)) {
		return dir;
	}

	return opposite(dir);
}

Cell& Model::getCell(const Pos& pos) {
	return grid_(pos.row, pos.col);
}

bool Model::isValid(const Pos& pos) const {
	// Note: grid width - height is reversed
	return
		pos.row >= 0 && pos.row < kMaxRows &&
		pos.col >= 0 && pos.col < kMaxCols;
}

int Model::getCoverage() const {
	int count = 0;
	for (auto& cell : grid_) {
		if (cell.owner == 1) {
			++count;
		}
	}
	return count;
}

} // namespace evil
