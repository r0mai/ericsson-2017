#pragma once

#include "Matrix.h"
#include "Protocol.h"
#include "Utils.h"
#include "Direction.h"

#include <vector>
#include <iostream>
#include <memory>
#include <climits>


namespace evil {

template<typename T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& mat) {
	for (int row = 0; row < mat.rows(); ++row) {
		for (int col = 0; col < mat.cols(); ++col) {
			out << mat(row, col);
		}
		out << std::endl;
	}
	return out;
}

struct Unit {
	int index = -1;
	int owner = -1;
	int health = -1;
	int killer = -1; // ???

	Pos pos;
	Direction dir;

	// algo
	Pos next_pos;
};

struct EnemyState {
	Pos pos;

	Direction h_dir;
	Direction v_dir;
};

inline
bool operator<(const EnemyState& lhs, const EnemyState& rhs) {
	return
		std::tie(lhs.pos, lhs.h_dir, lhs.v_dir) <
		std::tie(rhs.pos, rhs.h_dir, rhs.v_dir);
}

inline
bool operator==(const EnemyState& lhs, const EnemyState& rhs) {
	return
		std::tie(lhs.pos, lhs.h_dir, lhs.v_dir) ==
		std::tie(rhs.pos, rhs.h_dir, rhs.v_dir);
}

struct UnitState {
	Pos pos;
	int owner = -1;
};

struct Enemy : EnemyState {
	Enemy() = default;
	Enemy(const EnemyState& state) : EnemyState(state) {}

	// other non-model stuff can go in here
};

struct Cell {
	int owner = 0;
	int attacking_unit = -1;

	bool can = true;

	bool isAttacked() const { return attacking_unit >= 0; }
	bool isOurs() const { return is_ours; }

	// algo
	int color = 0;
	int proximity = 0;
	bool is_ours = false;
};

Direction fromDirection(protocol::Direction dir);
protocol::Direction toProtocolDirection(Direction dir);

bool operator!=(const Pos& lhs, const Pos& rhs);
bool operator==(const Alignment& lhs, const Alignment& rhs);
bool operator!=(const Alignment& lhs, const Alignment& rhs);

std::ostream& operator<<(std::ostream& out, Direction dir);

class Model {
public:
	using Grid = Matrix<Cell>;
	using Moves = std::vector<std::pair<int, Direction>>;

	static constexpr int kMaxRows = 90;
	static constexpr int kMaxCols = 120;

	static Model fromResponse(protocol::Response::Reader response);
	std::unique_ptr<capnp::MallocMessageBuilder> toCapnp() const;

	bool isValid() const;
	bool isValid(const Pos& pos) const;
	Pos size() const;

	Grid& getGrid();
	const Grid& getGrid() const;
	std::vector<Unit>& getAllUnits();
	const std::vector<Unit>& getAllUnits() const;
	std::vector<Unit> getOurUnits() const;
	std::vector<Enemy>& getEnemies();
	const std::vector<Enemy>& getEnemies() const;
	const Cell& getCell(const Pos& pos) const;
	Cell& getCell(const Pos& pos);
	const std::string& getStatus() const;
	Unit& getUnit(int index);
	const Unit& getUnit(int index) const;

	int getTick() const;
	int getLevel() const;
	int getOwns() const;
	int getCoverage() const;
	int maxTick() const;

	std::string getTickInfo() const;

	void setTick(int tick);
	void setLevel(int level);
	void setOwns(int owns);

	std::vector<EnemyState> getInsideEnemies() const;
	std::vector<EnemyState> getOutsideEnemies() const;

	std::vector<EnemyState> getEnemiesUnderOwner(int owner) const;

	// after a step
	std::vector<EnemyState> possibleEnemyStates(const EnemyState& enemy) const;
	// after a step for all enemies
	std::vector<EnemyState> allPossibleEnemyStates() const;



	// returned vector size is (step + 1)
	std::vector<std::vector<EnemyState>> allPossibleEnemyStates(
		int step, int max_states = INT_MAX, bool* success = nullptr) const;
	std::vector<std::vector<EnemyState>> allPossibleEnemyStates(
		const std::vector<EnemyState>& states,
		int step, int max_states = INT_MAX, bool* success = nullptr) const;

	bool unitCanStepOn(const Pos& pos, int owner) const;
	std::vector<UnitState> possibleUnitStates(const UnitState& unit) const;

	// Enemy lookahead n steps
	// -1 means no reachable/don't know
	// 0 means enemy is there at the current tick
	// 0 < n <= lookahead means an enemy could be there after n ticks
	Matrix<int> lookaheadEnemies(
		int lookahead, int max_states = INT_MAX, bool* success = nullptr) const;
	Matrix<int> lookaheadEnemies(
		const std::vector<EnemyState>& states,
		int lookahead, int max_states = INT_MAX, bool* success = nullptr) const;

	// 0: notsafe
	// 1: safe, ours
	// 2: safe, not ours
	// 3: safe, cannot conquered
	Matrix<int> getColorizedGrid(int steps);
	int potentialWin(const AABB& rect, const Matrix<int>& colorized_grid);

	bool IsSafeToMoveOutAndBack(const Pos& pos) const;

	Direction SafeBlueMove(const Unit& unit, Direction dir) const;

	void addBorder(int owner = 1, int thickness = 2);
	void addStartingPoint(int owner, Pos origin);
	bool stepAsServer(std::mt19937& rng_engine);
	void stepEnemy(Enemy& enemy, std::mt19937& rng_engine);
	void stepUnit(int unit_idx);
	void killUnit(int unit_idx);
	void attackFinished(int unit, const Pos& last_attack_pos);

	std::vector<Direction> createEar(
		const Pos& initial_pos,
		Direction d1,
		Direction d2,
		int length_1,
		int length_2) const;

	Pos getSafeCorner() const;
	Pos getNiceCorner() const;

	// Time until enemy bounces at position. Negative for outsiders.
	std::pair<int, Pos> getEnemyBounce(const Enemy& enemy) const;

	Direction adjustDirection(int unit_index, Direction dir) const;
	void provision(const Moves& moves);

	Direction directionTowards(const Pos& src, const Pos& dst) const;

	void dumpStatus(std::ostream& out, bool newline=true);

private:
	void colorize();
	int getSafeDistance();
	Pos trace(const Pos& pos, Direction v_dir, Direction h_dir, int col);

	Grid grid_ {kMaxRows, kMaxCols};
	std::vector<Unit> units_;
	std::vector<Enemy> enemies_;

	std::string status_;
	int tick_ = -1;
	int level_ = -1;
	int owns_ = -1;
};




} // namespace evil
