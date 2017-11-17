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

protocol::Direction toProtocolDirection(Direction dir) {
	switch(dir) {
		case Direction::kLeft: return protocol::Direction::LEFT;
		case Direction::kRight: return protocol::Direction::LEFT;
		case Direction::kUp: return protocol::Direction::LEFT;
		case Direction::kDown: return protocol::Direction::LEFT;
		default: break;
	}
	assert(false);
	return protocol::Direction::DOWN;
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

bool operator==(const Pos& lhs, const Pos& rhs) {
	return lhs.row == rhs.row && lhs.col == rhs.col;
}

bool operator!=(const Pos& lhs, const Pos& rhs) {
	return !(lhs == rhs);
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

Model Model::fromResponse(protocol::Response::Reader response) {
	if (response.getCells().size() == 0) {
		return {};
	}

	Model m;
	auto& mat = m.grid_;
	int row = 0;

	m.status_ = response.getStatus().cStr();
	m.tick_ = response.getInfo().getTick();
	m.level_ = response.getInfo().getLevel();
	m.owns_ = response.getInfo().getOwns();

	for (auto cells : response.getCells()) {
		int col = 0;
		for (auto cell : cells) {
			mat(row, col).owner = cell.getOwner();
			mat(row, col).is_attacked = cell.getAttack().isUnit();
			++col;
		}
		++row;
	}
	m.units_.clear();
	m.enemies_.clear();

	for (auto enemy : response.getEnemies()) {
		auto pos = enemy.getPosition();
		Enemy e;
		e.pos = Pos{pos.getRow(), pos.getCol()};
		e.v_dir = fromDirection(enemy.getDirection().getVertical());
		e.h_dir = fromDirection(enemy.getDirection().getHorizontal());
		m.enemies_.push_back(e);
	}

	for (auto unit : response.getUnits()) {
		auto pos = unit.getPosition();
		Unit u;
		u.pos = Pos{pos.getRow(), pos.getCol()};
		u.dir = fromDirection(unit.getDirection());
		m.units_.push_back(u);
	}

	m.colorize();
	return m;
}

std::unique_ptr<capnp::MallocMessageBuilder> Model::toCapnp() const {
	auto builder = std::make_unique<capnp::MallocMessageBuilder>();
	auto response = builder->initRoot<protocol::Response>();
	auto info = response.initInfo();

	response.setStatus("status from localhost");

	info.setTick(tick_);
	info.setLevel(level_);
	info.setOwns(owns_);

	auto cells = response.initCells(grid_.height());
	for (int r = 0; r < grid_.height(); ++r) {
		auto row = cells.init(r, grid_.width());
		for (int c = 0; c < grid_.height(); ++c) {
			auto cell = row[c];
			auto& m_cell = grid_(r, c);
			cell.setOwner(m_cell.owner);

			// TODO WTF
			auto attack = cell.initAttack();
			if (m_cell.is_attacked) {
				attack.setUnit(0);
			}
		}
	}

	auto units = response.initUnits(units_.size());
	for (int i = 0; i < units_.size(); ++i) {
		auto pos = units[i].initPosition();
		pos.setRow(units_[i].pos.row);
		pos.setCol(units_[i].pos.col);

		units[i].setOwner(units_[i].owner);
		units[i].setHealth(units_[i].health);
		units[i].setKiller(units_[i].killer);
		units[i].setDirection(toProtocolDirection(units_[i].dir));
	}

	auto enemies = response.initEnemies(enemies_.size());
	for (int i = 0; i < enemies_.size(); ++i) {
		auto pos = enemies[i].initPosition();
		pos.setRow(enemies_[i].pos.row);
		pos.setCol(enemies_[i].pos.col);

		auto dir = enemies[i].initDirection();
		dir.setVertical(toProtocolDirection(enemies_[i].v_dir));
		dir.setHorizontal(toProtocolDirection(enemies_[i].h_dir));
	}

	return builder;
}

bool Model::isValid() const {
	return getUnits().size() > 0;
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

Direction Model::adjustDirection(int unit_index, Direction dir) const {
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
