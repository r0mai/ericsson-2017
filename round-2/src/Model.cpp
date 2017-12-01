#include "Model.h"
#include <cassert>
#include <limits>
#include <sstream>


namespace evil {

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

bool operator==(const Alignment& lhs, const Alignment& rhs) {
	return lhs.axis0 == rhs.axis0 && lhs.axis1 == rhs.axis1;
}

bool operator!=(const Alignment& lhs, const Alignment& rhs) {
	return !(lhs == rhs);
}

Model::Grid& Model::getGrid() {
	return grid_;
}

const Model::Grid& Model::getGrid() const {
	return grid_;
}

std::vector<Unit>& Model::getAllUnits() {
	return units_;
}

const std::vector<Unit>& Model::getAllUnits() const {
	return units_;
}

std::vector<Unit> Model::getOurUnits() const {
	std::vector<Unit> units;
	for (auto& unit : getAllUnits()) {
		if (unit.owner == getOwns()) {
			units.push_back(unit);
		}
	}
	return units;
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

void Model::setOwns(int owns) {
	owns_ = owns;
}

std::vector<EnemyState> Model::getInsideEnemies() const {
	return getEnemiesUnderOwner(0);
}

std::vector<EnemyState> Model::getOutsideEnemies() const {
	return getEnemiesUnderOwner(1);
}

std::vector<EnemyState> Model::getEnemiesUnderOwner(int owner) const {
	std::vector<EnemyState> states;
	for (auto& enemy : enemies_) {
		if (getCell(enemy.pos).owner == owner) {
			states.push_back(enemy);
		}
	}
	return states;
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
			auto& m_cell = mat(row, col);
			m_cell.owner = cell.getOwner();
			m_cell.is_ours = m_cell.owner == m.owns_;
			if (cell.getAttack().isUnit()) {
				m_cell.attacking_unit = cell.getAttack().getUnit();
			} else if (cell.getAttack().isCan()) {
				m_cell.can = cell.getAttack().getCan();
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

	int index = 0;
	for (auto unit : response.getUnits()) {
		auto pos = unit.getPosition();
		Unit u;
		u.index = index;
		++index;
		u.pos = Pos{pos.getRow(), pos.getCol()};
		u.dir = fromDirection(unit.getDirection());
		u.health = unit.getHealth();
		u.next_pos = u.pos;
		u.owner = unit.getOwner();
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
			} else {
				attack.setCan(m_cell.can);
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
	return getAllUnits().size() > 0;
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

#if 0
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
#endif

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

void Model::addStartingPoint(int owner, Pos origin) {
	for (int r = origin.row; r < origin.row + 4; ++r) {
		for (int c = origin.col; c < origin.col + 4; ++c) {
			auto& cell = grid_(r, c);
			cell.owner = owner;
			cell.can = true;
		}
	}

	for (int r = origin.row + 1; r < origin.row + 3; ++r) {
		for (int c = origin.col + 1; c < origin.col + 3; ++c) {
			auto& cell = grid_(r, c);
			cell.can = false;
		}
	}

	Unit u1;
	u1.owner = owner;
	u1.health = 3;
	u1.killer = 3;
	u1.dir = Direction::kDown;

	Unit u2 = u1;

	u1.pos = origin;
	u2.pos = origin;

	u1.pos.row += 1;
	u1.pos.col += 1;

	u2.pos.row += 1;
	u2.pos.col += 2;

	units_.push_back(u1);
	units_.push_back(u2);

	Enemy e1;
	e1.h_dir = Direction::kRight;
	e1.v_dir = Direction::kDown;
	e1.pos = origin;
	e1.pos.row += 0;
	e1.pos.col += 1;

	Enemy e2;
	e2.h_dir = Direction::kLeft;
	e2.v_dir = Direction::kDown;
	e2.pos = origin;
	e2.pos.row += 1;
	e2.pos.col += 3;

	Enemy e3;
	e3.h_dir = Direction::kLeft;
	e3.v_dir = Direction::kDown;
	e3.pos = origin;
	e3.pos.row += 3;
	e3.pos.col += 2;

	Enemy e4;
	e4.h_dir = Direction::kLeft;
	e4.v_dir = Direction::kDown;
	e4.pos.row += 2;
	e4.pos.col += 0;

	enemies_.push_back(e1);
	enemies_.push_back(e2);
	enemies_.push_back(e3);
	enemies_.push_back(e4);
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

	for (auto& enemy : enemies_) {
		for (int i = 0; i < units_.size(); ++i) {
			if (units_[i].pos == enemy.pos) {
				killUnit(i);
			}
		}
	}

	return true;
}

void Model::attackFinished(int unit_idx, const Pos& last_attack_pos) {
	enum class AttackState {
		kUnknonwn,
		kAttacked,
		kCantCapture,
		kCaptured
	};

	Matrix<AttackState> attack_grid(grid_.rows(), grid_.cols(), AttackState::kUnknonwn);

	auto attacked_grid = floodFill(grid_, last_attack_pos,
		[&](const Pos& pos) {
			return getCell(pos).attacking_unit == unit_idx;
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

	auto& unit = getUnit(unit_idx);
	for (auto attacked : attacked_poss) {
		auto f = [&](const Pos& p) {
			if (attack_grid(p.row, p.col) == AttackState::kUnknonwn) {
				auto try_capture_grid = floodFill(grid_, p,
					[&](const Pos& pos) {
						auto& cell = getCell(pos);
						return !cell.isAttacked() && cell.owner != unit.owner;
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
				grid_(r, c).owner = unit.owner;
			} else if (attack_grid(r, c) == AttackState::kAttacked) {
				grid_(r, c).owner = unit.owner;
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

	for (int r = 0; r < grid_.rows(); ++r) {
		for (int c = 0; c < grid_.cols(); ++c) {
			auto& cell = grid_(r, c);
			if (!cell.can && cell.owner == unit.owner) {
				unit.pos = {r, c};
				goto after_loop;
			}
		}
	}
after_loop:

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

	Direction cw_h;
	Direction cw_v;
	Direction ccw_h;
	Direction ccw_v;

	std::tie(cw_h, cw_v) = rotateCW(h, v);
	std::tie(ccw_h, ccw_v) = rotateCCW(h, v);

	assert(cw_h == Direction::kLeft || cw_h == Direction::kRight);
	assert(ccw_h == Direction::kLeft || ccw_h == Direction::kRight);
	assert(cw_v == Direction::kUp || cw_v == Direction::kDown);
	assert(ccw_v == Direction::kUp || ccw_v == Direction::kDown);

	const auto& old_cell = getCell(enemy.pos);

	auto between = [](const Pos& p1, const Pos& p2) -> Pos {
		assert(p1.row == p2.row || p1.col == p2.col);
		assert(taxicabDistance(p1, p2) == 2);

		return {(p1.row + p2.row) / 2, (p1.col + p2.col) / 2};
	};

	// LL LF FF
	// LB ^> RF
	// BB RB RR

	auto forward_p   = neighbor(neighbor(enemy.pos,  v),  h);
	auto backwards_p = neighbor(neighbor(enemy.pos, ov), oh);
	auto right_p     = neighbor(neighbor(enemy.pos, cw_v), cw_h);
	auto left_p      = neighbor(neighbor(enemy.pos, ccw_v), ccw_h);
	auto rf_p        = between(forward_p, right_p);
	auto lf_p        = between(forward_p, left_p);
	auto rb_p        = between(backwards_p, right_p);
	auto lb_p        = between(backwards_p, left_p);

	auto pos_ok = [&](const Pos& p) {
		return isValid(p) && old_cell.owner == getCell(p).owner && getCell(p).can;
	};

	bool forward_b = pos_ok(forward_p);
	bool backwards_b = pos_ok(backwards_p);
	bool right_b = pos_ok(right_p);
	bool left_b = pos_ok(left_p);
	bool rf_b = pos_ok(rf_p);
	bool lf_b = pos_ok(lf_p);
	bool rb_b = pos_ok(rb_p);
	bool lb_b = pos_ok(lb_p);

	// case 1
	// can we continue moving forward
	if (forward_b) {
		return {{forward_p, h, v}};
	}

	std::vector<EnemyState> possible_states;
	possible_states.reserve(8);

	// case 2, 3, 4
	if (left_b && lf_b) {
		possible_states.push_back({left_p, ccw_h, ccw_v});
	}
	if (right_b && rf_b) {
		possible_states.push_back({right_p, cw_h, cw_v});
	}
	if (!possible_states.empty() && backwards_b) {
		possible_states.push_back({backwards_p, oh, ov});
	}

	if (!possible_states.empty()) {
		return possible_states;
	}

	// case 5
	if (!backwards_b && !forward_b && !left_b && !right_b && !lf_b && !rf_b) {
		if (lb_b) {
			possible_states.push_back({lb_p, oh, ov});
		}
		if (rb_b) {
			possible_states.push_back({rb_p, oh, ov});
		}
	}

	if (!possible_states.empty()) {
		return possible_states;
	}

	// case 6
	if (backwards_b) {
		possible_states.push_back({backwards_p, oh, ov});
	}
	if (left_b) {
		possible_states.push_back({left_p, ccw_h, ccw_v});
	}
	if (right_b) {
		possible_states.push_back({right_p, cw_h, cw_v});
	}
	if (lf_b) {
		possible_states.push_back({lf_p, ccw_h, ccw_v});
	}
	if (rf_b) {
		possible_states.push_back({rf_p, cw_h, cw_v});
	}

	if (possible_states.empty()) {
		std::cerr << "Possible states empty "
			<< enemy.pos << ", " << enemy.h_dir << ", " << enemy.v_dir << std::endl;
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
	std::vector<EnemyState> initial_states(enemies_.size());
	for (int i = 0; i < enemies_.size(); ++i) {
		initial_states[i] = enemies_[i];
	}
	return allPossibleEnemyStates(initial_states, step, max_states, success);
}

std::vector<std::vector<EnemyState>> Model::allPossibleEnemyStates(
	const std::vector<EnemyState>& initial_states,
	int step, int max_states, bool* success) const
{
	if (success) { *success = false; }

	int count_all_states = 0;
	std::vector<std::vector<EnemyState>> states;
	states.reserve(step + 1);
	states.push_back(initial_states);

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
	std::vector<EnemyState> initial_states(enemies_.size());
	for (int i = 0; i < enemies_.size(); ++i) {
		initial_states.push_back(enemies_[i]);
	}
	return lookaheadEnemies(initial_states, lookahead, max_states, success);
}

Matrix<int> Model::lookaheadEnemies(
	const std::vector<EnemyState>& initial_states,
	int lookahead, int max_states, bool* success) const
{
	auto states = allPossibleEnemyStates(initial_states, lookahead, max_states, success);
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

	for (int row = 0; row < grid_.rows(); ++row) {
		for (int col = 0; col < grid_.cols(); ++col) {
			for (auto& unit : getAllUnits()) {
				if (unit.owner == getOwns()) {
					continue;
				}

				Pos p{row, col};
				auto d = taxicabDistance(p, unit.pos);
				if (d <= lookahead) {
					int& cell = lookahead_matrix(p.row, p.col);
					cell = lookahead;
				}
			}
		}
	}

	return lookahead_matrix;
}

bool Model::IsSafeToMoveOutAndBack(const Pos& pos) const {
	auto enemy_states = allPossibleEnemyStates(2);
	for (int i = 1; i <= 2; ++i) {
		for (auto& enemy_state : enemy_states[i]) {
			if (enemy_state.pos == pos) {
				return false;
			}
		}
	}
	return true;
}

Direction Model::SafeBlueMove(const Unit& unit, Direction dir) const {
	auto la = lookaheadEnemies(getEnemiesUnderOwner(unit.owner), 1);

	std::vector<Direction> directions;
	directions.reserve(4);

	if (dir == Direction::kNone) {
		dir = Direction::kLeft;
	}

	directions.push_back(dir);
	directions.push_back(rotateCW(dir));
	directions.push_back(rotateCCW(dir));
	directions.push_back(opposite(dir));

	for (auto d : directions) {
		auto p = neighbor(unit.pos, d);
		if (isValid(p) && getCell(p).owner == unit.owner && la(p.row, p.col) != 1) {
			return d;
		}
	}

	std::cout << "No safe moves from SafeBlueMove" << std::endl;
	// nothing seems to be safe
	return dir;
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

int Model::maxTick() const {
	return 1000 + (level_ % 500) * 10;
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
	if (source_pos == target_pos) {
		return Direction::kNone;
	}

	auto dst_matrix = distanceFill(target_pos, size(),
		[&](const Pos& p) -> bool {
			auto& cell = getCell(p);
			return cell.isOurs();
		});

	auto dst = dst_matrix.at(source_pos);
	auto closer_dir = Direction::kNone;

	if (dst == -1) {
		// source is not on blue
		dst = std::numeric_limits<int>::max();

		for (auto nb_dir : getDirections(source_pos, size())) {
			auto nb_pos = neighbor(source_pos, nb_dir);
			for (int row = 0; row < grid_.rows(); ++row) {
				for (int col = 0; col < grid_.cols(); ++col) {
					auto nb_dst = dst_matrix.at({row, col});
					if (nb_dst == -1) {
						continue;
					}

					nb_dst +=
						2 * std::abs(nb_pos.row - row) +
						2 * std::abs(nb_pos.col - col);
					if (nb_dst < dst) {
						closer_dir = nb_dir;
						dst = nb_dst;
					}
				}
			}
		}
	} else {
		for (auto nb_dir : getDirections(source_pos, size())) {
			auto nb_pos = neighbor(source_pos, nb_dir);
			auto nb_dst = dst_matrix.at(nb_pos);
			if (nb_dst == -1) {
				continue;
			}

			if (nb_dst < dst) {
				closer_dir = nb_dir;
			}
		}
	}

	return closer_dir;
}

std::string Model::getTickInfo() const {
	std::stringstream ss;
	ss << "L " << level_ << " ";
	ss << "T " << tick_ << " ";
	ss << "C " << getCoverage() << " ";
	ss << "H";
	for (auto& unit : getOurUnits()) {
		ss << " " << unit.health;
		ss << " " << unit.dir;
	}

	ss << " ";
	ss << status_;
	return ss.str();
}

std::vector<Direction> Model::createEar(
	const Pos& initial_pos,
	Direction d1,
	Direction d2,
	int length_1,
	int length_2) const
{
	auto od1 = opposite(d1);
	auto od2 = opposite(d2);

	std::vector<Direction> dirs;

	for (int i = 0; i < length_1; ++i) {
		dirs.push_back(d1);
	}
	for (int i = 0; i < length_2; ++i) {
		dirs.push_back(d2);
	}
	for (int i = 0; i < length_1; ++i) {
		dirs.push_back(od1);
	}
	for (int i = 0; i < length_2; ++i) {
		dirs.push_back(od2);
	}

	return dirs;
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

std::pair<int, Pos> Model::getEnemyBounce(const Enemy& enemy) const {
	// Note: this could return {-1, pos} for outside bouncers

	int t = -1;
	auto pos = enemy.pos;
	auto v_dir = enemy.v_dir;
	auto h_dir = enemy.h_dir;
	Pos last = pos;

	while (getCell(pos).owner != 1) {
		++t;
		last = pos;
		pos = neighbor(pos, v_dir);
		pos = neighbor(pos, h_dir);
	}

	return {t, last};
}

std::vector<UnitState> Model::possibleUnitStates(const UnitState& unit) const {
	std::vector<UnitState> states;

	states.reserve(4);
	auto pos = unit.pos;
	for (int vert = -1; vert <= 1; vert+=2) {
		auto new_pos = pos;
		pos.row += vert;
		if (unitCanStepOn(new_pos, unit.owner)) {
			states.push_back(UnitState{new_pos, unit.owner});
		}
	}
	for (int hor = -1; hor <= 1; hor+=2) {
		auto new_pos = pos;
		pos.col += hor;
		if (unitCanStepOn(new_pos, unit.owner)) {
			states.push_back(UnitState{new_pos, unit.owner});
		}
	}

	return states;
}

bool Model::unitCanStepOn(const Pos& pos, int owner) const {
	auto& cell = grid_.at(pos);
	return (
		pos.row >= 0 && pos.row < grid_.rows() && pos.col >= 0 &&
		pos.col < grid_.cols()) && (cell.can || cell.owner == owner);
}

Pos Model::getSafeCorner() const {
	for (int row = 0; row < kMaxRows; ++row) {
		for (int col = 0; col < kMaxCols; ++col) {
			auto pos = Pos{row, col};
			auto& cell = getCell(pos);
			if (cell.isOurs() && !cell.can) {
				return pos;
			}
		}
	}
	return {};
}

Matrix<int> Model::getColorizedGrid(int steps) {
	auto cgrid = lookaheadEnemies(steps);
	for (int row = 0; row < cgrid.rows(); ++row) {
		for (int col = 0; col < cgrid.cols(); ++col) {
			auto& cell = getCell(Pos{row, col});
			auto& colored_cell = cgrid.at(row, col);
			if (colored_cell != -1 || cell.attacking_unit != -1 || (!cell.can && cell.owner != getOwns())) {
				colored_cell = 0;
			} else if (cell.owner == getOwns()) {
				colored_cell = 1;
			} else {
				colored_cell = 2;
			}
		}
	}

	return cgrid;
}
int Model::potentialWin(const AABB& rect, const Matrix<int>& colorized_grid) {
	int sum = 0;
	bool has_owned = false;
	for (int row = rect.mins.row; row <= rect.maxs.row; ++row) {
		for (int col = rect.mins.col; col <= rect.maxs.col; ++col) {
			auto& cell = colorized_grid.at(row, col);
			if (cell == 0) {
				return 0;
			} else if (cell == 1) {
				has_owned = true;
			} else {
				++sum;
			}
		}
	}

	return has_owned ? sum : 0;
}

} // namespace evil
