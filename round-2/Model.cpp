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
		case Direction::kRight: return protocol::Direction::RIGHT;
		case Direction::kUp: return protocol::Direction::UP;
		case Direction::kDown: return protocol::Direction::DOWN;
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

std::vector<Unit>& Model::getUnits() {
	return units_;
}

const std::vector<Unit>& Model::getUnits() const {
	return units_;
}

std::vector<Enemy>& Model::getEnemies() {
	return enemies_;
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
	Model m;

	m.status_ = response.getStatus().cStr();
	m.tick_ = response.getInfo().getTick();
	m.level_ = response.getInfo().getLevel();
	m.owns_ = response.getInfo().getOwns();

	if (response.getCells().size() == 0) {
		return m;
	}

	auto& mat = m.grid_;
	int row = 0;

	for (auto cells : response.getCells()) {
		int col = 0;
		for (auto cell : cells) {
			mat(row, col).owner = cell.getOwner();
			mat(row, col).is_attacked = cell.getAttack().isUnit();
			++col;
		}
		++row;
	}

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

	response.setStatus(status_);

	info.setTick(tick_);
	info.setLevel(level_);
	info.setOwns(owns_);

	auto cells = response.initCells(grid_.rows());
	for (int r = 0; r < grid_.rows(); ++r) {
		auto row = cells.init(r, grid_.cols());
		for (int c = 0; c < grid_.cols(); ++c) {
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
		auto vd = enemy.v_dir;
		auto hd = enemy.h_dir;
		auto vdx = opposite(vd);
		auto hdx = opposite(hd);

		auto bounce1 = trace(enemy.pos, vd, hd, 5);
		auto bounce2a = trace(bounce1, vdx, hd, 6);
		auto bounce2b = trace(bounce1, vd, hdx, 6);
		auto bounce2c = trace(enemy.pos, vdx, hdx, 6);

		auto bounce3aa = trace(bounce2a, vd, hd, 7);
		auto bounce3ab = trace(bounce2a, vdx, hdx, 7);
		auto bounce3ba = trace(bounce2b, vd, hd, 7);
		auto bounce3bb = trace(bounce2b, vdx, hdx, 7);
		auto bounce3ca = trace(bounce2c, vd, hdx, 7);
		auto bounce3cb = trace(bounce2c, vdx, hd, 7);
	}

	for (auto& enemy : enemies_) {
		auto pos = enemy.pos;
		getCell(pos).color = 4;
	}
}

Pos Model::trace(const Pos& start, Direction v_dir, Direction h_dir, int col) {
	Pos pos = start;
	Pos last = start;
	bool overdraw = col <= 5;

	while (getCell(pos).owner != 1) {
		auto& cc = getCell(pos).color;
		if (overdraw || cc == 0) {
			cc = col;
		}
		last = pos;
		pos = neighbor(pos, v_dir);
		pos = neighbor(pos, h_dir);
	}
	return last;
}


void Model::addBorder(int owner, int thickness) {
	for (int i = 0; i < thickness; ++i) {
		for (int r = 0; r < grid_.rows(); ++r) {
			grid_(r, i).owner = owner;
			grid_(r, grid_.cols() - 1 - i).owner = owner;
		}
		for (int c = 0; c < grid_.cols(); ++c) {
			grid_(i, c).owner = owner;
			grid_(grid_.rows() - 1 - i, c).owner = owner;
		}
	}
}

bool Model::stepAsServer(std::mt19937& rng_engine) {
	status_ = {};

	for (auto& enemy : enemies_) {
		stepEnemy(enemy, rng_engine);
	}

	for (auto& unit : units_) {
		auto new_pos = neighbor(unit.pos, unit.dir);
		if (!isValid(new_pos)) {
			status_ = "Unit out of bounds";
			return false;
		}
		unit.pos = new_pos;
	}

	return true;
}

void Model::stepEnemy(Enemy& enemy, std::mt19937& rng_engine) {
	auto v = enemy.v_dir;
	auto h = enemy.h_dir;
	auto ov = opposite(enemy.v_dir);
	auto oh = opposite(enemy.h_dir);

	const auto& old_cell = getCell(enemy.pos);

	auto forward_p   = neighbor(neighbor(enemy.pos,  v),  h);
	auto backwards_p = neighbor(neighbor(enemy.pos, ov), oh);
	auto side1_p     = neighbor(neighbor(enemy.pos,  v), oh);
	auto side2_p     = neighbor(neighbor(enemy.pos, ov),  h);

	const auto& forward_c = getCell(forward_p);
	const auto& backwards_c = getCell(backwards_p);
	const auto& side1_c = getCell(side1_p);
	const auto& side2_c = getCell(side2_p);

	// can we continue moving forward
	if (old_cell.owner == forward_c.owner) {
		enemy.pos = forward_p;
		return;
	}

	using EnemyState = std::tuple<Pos, Direction, Direction>;
	std::vector<EnemyState> possible_states;
	possible_states.reserve(3);

	if (old_cell.owner == backwards_c.owner) {
		possible_states.push_back(std::make_tuple(backwards_p, ov, oh));
	}
	if (old_cell.owner == side1_c.owner) {
		possible_states.push_back(std::make_tuple(side1_p, v, oh));
	}
	if (old_cell.owner == side2_c.owner) {
		possible_states.push_back(std::make_tuple(side2_p, ov, h));
	}

	if (!possible_states.empty()) {
		auto& state = possible_states[getRandom(rng_engine, 0, possible_states.size() - 1)];
		enemy.pos = std::get<0>(state);
		enemy.v_dir = std::get<1>(state);
		enemy.h_dir = std::get<2>(state);
	}

	// TODO implement super edge cases
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
