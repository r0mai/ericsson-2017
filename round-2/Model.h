#pragma once

#include "Matrix.h"
#include "Protocol.h"
#include "Utils.h"
#include "Pos.h"

#include <vector>
#include <iostream>
#include <memory>

template<typename T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& mat) {
	for (int row = 0; row < mat.width(); ++row) {
		for (int col = 0; col < mat.height(); ++col) {
			out << mat(row, col);
		}
		out << std::endl;
	}
	return out;
}


namespace evil {

enum class Direction {
	kUp,
	kRight,
	kDown,
	kLeft,

	kNone
};

struct Unit {
	int owner = -1;
	int health = -1;
	int killer = -1; // ???

	Pos pos;
	Direction dir;

	// algo
	Pos next_pos;
};

struct Enemy {
	Pos pos;

	Direction h_dir;
	Direction v_dir;
};

struct Cell {
	int owner = 0;
	int attacking_unit = -1;

	bool isAttacked() const { return attacking_unit >= 0; }

	// algo
	int color = 0;
};

Direction opposite(Direction dir);
Pos neighbor(const Pos& pos, Direction dir);
Direction fromDirection(protocol::Direction dir);
protocol::Direction toProtocolDirection(Direction dir);

bool operator==(const Pos& lhs, const Pos& rhs);
bool operator!=(const Pos& lhs, const Pos& rhs);

std::ostream& operator<<(std::ostream& out, const Pos& pos);
std::ostream& operator<<(std::ostream& out, Direction dir);



class Model {
public:
	using Grid = Matrix<Cell>;
	using Moves = std::vector<std::pair<int, Direction>>;

	static constexpr int kMaxRows = 80;
	static constexpr int kMaxCols = 100;

	static Model fromResponse(protocol::Response::Reader response);
	std::unique_ptr<capnp::MallocMessageBuilder> toCapnp() const;

	bool isValid() const;

	const Grid& getGrid() const;
	std::vector<Unit>& getUnits();
	const std::vector<Unit>& getUnits() const;
	std::vector<Enemy>& getEnemies();
	const std::vector<Enemy>& getEnemies() const;
	const std::string& getStatus() const;

	int getTick() const;
	int getLevel() const;
	int getOwns() const;
	int getCoverage() const;

	void addBorder(int owner = 1, int thickness = 2);
	bool stepAsServer(std::mt19937& rng_engine);
	void stepEnemy(Enemy& enemy, std::mt19937& rng_engine);

	Direction adjustDirection(int unit_index, Direction dir) const;
	void provision(const Moves& moves);

private:
	void colorize();
	Pos trace(const Pos& pos, Direction v_dir, Direction h_dir, int col);

	Cell& getCell(const Pos& pos);
	bool isValid(const Pos& pos) const;

	Grid grid_ {kMaxRows, kMaxCols};
	std::vector<Unit> units_;
	std::vector<Enemy> enemies_;

	std::string status_;
	int tick_ = -1;
	int level_ = -1;
	int owns_ = -1;
};

} // namespace evil
