#include "Model.h"
#include <cassert>
#include <limits>


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

void Model::setTick(int tick) {
	tick_ = tick;
}

void Model::setLevel(int level) {
	level_ = level;
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
			if (cell.getAttack().isUnit()) {
				mat(row, col).attacking_unit = cell.getAttack().getUnit();
			}
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
		u.health = unit.getHealth();
		u.next_pos = u.pos;
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

			auto attack = cell.initAttack();
			if (m_cell.isAttacked()) {
				attack.setUnit(m_cell.attacking_unit);
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
	auto safe_dst = getSafeDistance();

	for (auto& cell : grid_) {
		cell.color = 0;
		cell.proximity = std::numeric_limits<int>::max();
		if (cell.owner == 1) {
			cell.color = 1;
		}
		if (cell.isAttacked()) {
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

		getCell(enemy.pos).proximity = - safe_dst;
		auto bounce1 = trace(enemy.pos, vd, hd, 5);

		auto bounce2a = trace(bounce1, vdx, hd, 6);
		auto bounce2b = trace(bounce1, vd, hdx, 6);
		auto bounce2c = trace(bounce1, vdx, hdx, 6);

		auto bounce3aa = trace(bounce2a, vd, hd, 7);
		auto bounce3ab = trace(bounce2a, vdx, hdx, 7);
		auto bounce3ba = trace(bounce2b, vd, hd, 7);
		auto bounce3bb = trace(bounce2b, vdx, hdx, 7);
		auto bounce3ca = trace(bounce2c, vd, hdx, 7);
		auto bounce3cb = trace(bounce2c, vdx, hd, 7);

		auto bounce4aaa = trace(bounce3aa, vdx, hd, 7);
		auto bounce4aab = trace(bounce3aa, vd, hdx, 7);
		auto bounce4aba = trace(bounce3ab, vdx, hd, 7);
		auto bounce4abb = trace(bounce3ab, vd, hdx, 7);
		auto bounce4baa = trace(bounce3ba, vdx, hd, 7);
		auto bounce4bab = trace(bounce3ba, vd, hdx, 7);
		auto bounce4bba = trace(bounce3bb, vdx, hd, 7);
		auto bounce4bbb = trace(bounce3bb, vd, hdx, 7);
		auto bounce4caa = trace(bounce3ca, vd, hd, 7);
		auto bounce4cab = trace(bounce3ca, vdx, hdx, 7);
		auto bounce4cba = trace(bounce3cb, vd, hd, 7);
		auto bounce4cbb = trace(bounce3cb, vdx, hdx, 7);
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
	int proximity = getCell(start).proximity;

	while (getCell(pos).owner != 1) {
		auto& cell = getCell(pos);
		auto& cc = cell.color;
		cell.proximity = std::min(proximity, cell.proximity);
		if (overdraw || cc == 0) {
			cc = col;
		}
		last = pos;
		pos = neighbor(pos, v_dir);
		pos = neighbor(pos, h_dir);
		++proximity;
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

	++tick_;

	for (auto& enemy : enemies_) {
		stepEnemy(enemy, rng_engine);
		auto& cell = getCell(enemy.pos);
		if (cell.isAttacked()) {
			killUnit(cell.attacking_unit);
		}
	}

	for (int i = 0; i < units_.size(); ++i) {
		stepUnit(i);
	}

	return true;
}

void Model::attackFinished(int unit, const Pos& last_attack_pos) {
	enum class AttackState {
		kUnknonwn,
		kAttacked,
		kCantCapture,
		kCaptured
	};

	Matrix<AttackState> attack_grid(grid_.rows(), grid_.cols(), AttackState::kUnknonwn);

	auto attacked_grid = floodFill(grid_, last_attack_pos,
		[&](const Pos& pos) {
			return getCell(pos).attacking_unit == unit;
		}
	);

	std::vector<Pos> attacked_poss;

	for (int r = 0; r < grid_.rows(); ++r) {
		for (int c = 0; c < grid_.cols(); ++c) {
			if (attacked_grid(r, c)) {
				attack_grid(r, c) = AttackState::kAttacked;
				attacked_poss.emplace_back(r, c);
			}
		}
	}

	for (auto attacked : attacked_poss) {
		auto f = [&](const Pos& p) {
			if (attack_grid(p.row, p.col) == AttackState::kUnknonwn) {
				auto try_capture_grid = floodFill(grid_, p,
					[&](const Pos& pos) {
						auto& cell = getCell(pos);
						return !cell.isAttacked() && cell.owner == 0;
					}
				);
				for (int r = 0; r < grid_.rows(); ++r) {
					for (int c = 0; c < grid_.cols(); ++c) {
						if (try_capture_grid(r, c)) {
							attack_grid(r, c) = AttackState::kCaptured;
						}
					}
				}
			}
		};

		f(neighbor(attacked, Direction::kUp));
		f(neighbor(attacked, Direction::kDown));
		f(neighbor(attacked, Direction::kLeft));
		f(neighbor(attacked, Direction::kRight));
	}

	for (auto& enemy : enemies_) {
		if (attack_grid(enemy.pos.row, enemy.pos.col) != AttackState::kCaptured) {
			continue;
		}
		auto cant_capture_grid = floodFill(grid_, enemy.pos, [&](const Pos& p) {
			return attack_grid(p.row, p.col) == AttackState::kCaptured;
		});
		for (int r = 0; r < grid_.rows(); ++r) {
			for (int c = 0; c < grid_.cols(); ++c) {
				if (cant_capture_grid(r, c)) {
					attack_grid(r, c) = AttackState::kCantCapture;
				}
			}
		}
	}

	for (int r = 0; r < grid_.rows(); ++r) {
		for (int c = 0; c < grid_.cols(); ++c) {
			if (attack_grid(r, c) == AttackState::kCaptured) {
				grid_(r, c).owner = 1;
			} else if (attack_grid(r, c) == AttackState::kAttacked) {
				grid_(r, c).owner = 1;
				grid_(r, c).attacking_unit = -1;
			}
		}
	}
}

void Model::stepUnit(int unit_idx) {
	auto& unit = units_[unit_idx];
	auto old_pos = unit.pos;
	auto new_pos = neighbor(unit.pos, unit.dir);

	if (!isValid(new_pos)) {
		status_ = "Unit out of bounds";
		killUnit(unit_idx);
		return;
	}

	auto& old_cell = getCell(old_pos);
	auto& new_cell = getCell(new_pos);

	unit.pos = new_pos;
	if (new_cell.owner != unit.owner) {
		new_cell.attacking_unit = unit_idx;
	} else {
		// we were attack and got to a safe zone
		if (old_cell.attacking_unit == unit_idx) {
			attackFinished(unit_idx, old_pos);
		}
	}
}

void Model::killUnit(int unit_idx) {
	std::cout << "Unit " << unit_idx << " died" << std::endl;
	auto& unit = units_[unit_idx];

	unit.pos = {0, 0};
	unit.dir = Direction::kDown;

	for (auto& cell : grid_) {
		if (cell.attacking_unit == unit_idx) {
			cell.attacking_unit = -1;
		}
	}
}

std::vector<EnemyState> Model::possibleEnemyStates(const EnemyState& enemy) const {
	auto v = enemy.v_dir;
	auto h = enemy.h_dir;
	auto ov = opposite(enemy.v_dir);
	auto oh = opposite(enemy.h_dir);

	const auto& old_cell = getCell(enemy.pos);

	auto forward_p   = neighbor(neighbor(enemy.pos,  v),  h);
	auto backwards_p = neighbor(neighbor(enemy.pos, ov), oh);
	auto side1_p     = neighbor(neighbor(enemy.pos,  v), oh);
	auto side2_p     = neighbor(neighbor(enemy.pos, ov),  h);

	// can we continue moving forward
	if (isValid(forward_p) && old_cell.owner == getCell(forward_p).owner) {
		return {{forward_p, v, h}};
	}

	std::vector<EnemyState> possible_states;
	possible_states.reserve(3);

	if (isValid(backwards_p) && old_cell.owner == getCell(backwards_p).owner) {
		possible_states.push_back({backwards_p, ov, oh});
	}
	if (isValid(side1_p) && old_cell.owner == getCell(side1_p).owner) {
		possible_states.push_back({side1_p, v, oh});
	}
	if (isValid(side2_p) && old_cell.owner == getCell(side2_p).owner) {
		possible_states.push_back({side2_p, ov, h});
	}

	if (!possible_states.empty()) {
		return possible_states;
	}

	// edgy cases

	{
		auto lin_side1_p = neighbor(enemy.pos, ov);
		auto lin_side2_p = neighbor(enemy.pos, oh);

		if (isValid(lin_side1_p) && old_cell.owner == getCell(lin_side1_p).owner) {
			possible_states.push_back({lin_side1_p, ov, oh});
		}
		if (isValid(lin_side2_p) && old_cell.owner == getCell(lin_side2_p).owner) {
			possible_states.push_back({lin_side2_p, ov, oh});
		}
	}

	if (!possible_states.empty()) {
		return possible_states;
	}

	{
		auto lin_side3_p = neighbor(enemy.pos, v);
		auto lin_side4_p = neighbor(enemy.pos, h);

		if (isValid(lin_side3_p) && old_cell.owner == getCell(lin_side3_p).owner) {
			possible_states.push_back({lin_side3_p, v, oh});
		}
		if (isValid(lin_side4_p) && old_cell.owner == getCell(lin_side4_p).owner) {
			possible_states.push_back({lin_side4_p, ov, h});
		}
	}
	return possible_states;
}

std::vector<EnemyState> Model::allPossibleEnemyStates() const {
	std::vector<EnemyState> states;
	for (auto& enemy : enemies_) {
		auto enemy_states = possibleEnemyStates(enemy);
		states.insert(states.end(), enemy_states.begin(), enemy_states.end());
	}
	return states;
}

std::vector<std::vector<EnemyState>> Model::allPossibleEnemyStates(
	int step, int max_states, bool* success) const
{
	if (success) { *success = false; }

	int count_all_states = 0;
	std::vector<std::vector<EnemyState>> states;
	states.reserve(step + 1);
	states.emplace_back();

	std::vector<EnemyState> enemies(enemies_.size());
	for (int i = 0; i < enemies.size(); ++i) {
		states.back().push_back(enemies_[i]);
	}
	for (int i = 1; i <= step; ++i) {
		auto& prev_states = states.back();
		states.emplace_back();
		auto& current_states = states.back();
		for (auto& enemy : prev_states) {
			auto next_states = possibleEnemyStates(enemy);
			count_all_states += next_states.size();

			if (count_all_states > max_states) {
				// it takes too much time, bail out
				return states;
			}

			current_states.insert(
				current_states.end(),
				next_states.begin(), next_states.end()
			);
		}
	}
	if (success) { *success = true; }
	return states;
}

Matrix<int> Model::lookaheadEnemies(
	int lookahead, int max_states, bool* success) const
{
	auto states = allPossibleEnemyStates(lookahead, max_states, success);
	if (success && !*success) {
		return {};
	}

	Matrix<int> lookahead_matrix(grid_.rows(), grid_.cols(), -1);
	for (int i = 0; i <= lookahead; ++i) {
		for (auto& enemy : states[i]) {
			int& cell = lookahead_matrix(enemy.pos.row, enemy.pos.col);
			if (cell == -1 || cell < i) {
				cell = i;
			}
		}
	}

	return lookahead_matrix;
}

void Model::stepEnemy(Enemy& enemy, std::mt19937& rng_engine) {
	auto possible_states = possibleEnemyStates(enemy);

	if (!possible_states.empty()) {
		auto& state = possible_states[getRandom(rng_engine, 0, possible_states.size() - 1)];
		enemy = Enemy{state};
	}
}

Direction Model::adjustDirection(int unit_index, Direction dir) const {
	auto& unit = units_[unit_index];

	if (dir == Direction::kNone) {
		dir = unit.dir;
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

const Cell& Model::getCell(const Pos& pos) const {
	return grid_(pos.row, pos.col);
}

bool Model::isValid(const Pos& pos) const {
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

void Model::provision(const Moves& moves) {
	for (auto& move : moves) {
		auto& unit = units_[move.first];
		unit.next_pos = neighbor(unit.pos, move.second);
	}
}

int Model::getSafeDistance() {
	int dst = std::numeric_limits<int>::max();
	auto pos = units_[0].pos;
	for (int r = 0; r < kMaxRows; ++r) {
		for (int c = 0; c < kMaxCols; ++c) {
			if (getCell({r, c}).owner == 1) {
				dst = std::min(dst,
					std::abs(pos.row - r) + std::abs(pos.col - c));
			}
		}
	}
	return dst;
}

Direction Model::directionTowards(const Pos& source_pos, const Pos& target_pos) const {
	auto dst_matrix = distanceFill(target_pos, size(),
		[&](const Pos& p) -> bool {
			auto& cell = getCell(p);
			return cell.owner == 1;
		});

	auto pos = getUnit(0).pos;
	auto dst = dst_matrix(pos.row, pos.col);
	auto closer_dir = Direction::kNone;

	for (auto nb_dir : directions(source_pos, size())) {
		auto nb_pos = neighbor(source_pos, nb_dir);
		auto nb_dst = dst_matrix(nb_pos.row, nb_pos.col);
		if (nb_dst == -1) {
			continue;
		}

		if (nb_dst < dst) {
			closer_dir = nb_dir;
		}
	}

	return closer_dir;
}

void Model::dumpStatus(std::ostream& out) {
	out << "L " << level_ << " ";
	out << "T " << tick_ << " ";
	out << "C " << getCoverage() << " ";
	out << "H " << units_[0].health << " ";
	out << units_[0].dir << " ";
	out << status_ << std::endl;
}

Pos Model::size() const {
	return Pos(grid_.rows(), grid_.cols());
}

Unit& Model::getUnit(int index) {
	return units_[index];
}

const Unit& Model::getUnit(int index) const {
	return units_[index];
}

} // namespace evil
