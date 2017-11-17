#pragma once
#include "Matrix.h"
#include "Protocol.h"
#include <vector>
#include <iostream>


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

struct Pos {
	Pos() = default;
	Pos(int row, int col) : row(row), col(col) {}

	int row = 0;
	int col = 0;
};

enum class Direction {
	kUp,
	kRight,
	kDown,
	kLeft,

	kNone
};

struct Unit {
	Pos pos;
	Direction dir;
};

struct Enemy {
	Pos pos;

	Direction h_dir;
	Direction v_dir;
};

struct Cell {
	int owner = 0;
	bool is_attacked = false;

	// algo
	int color = 0;
};


Direction opposite(Direction dir);
Pos neighbor(const Pos& pos, Direction dir);
Direction fromDirection(protocol::Direction dir);

std::ostream& operator<<(std::ostream& out, const Pos& pos);
std::ostream& operator<<(std::ostream& out, Direction dir);


class Model {
public:
	static constexpr int kMaxRows = 80;
	static constexpr int kMaxCols = 100;

	using Grid = Matrix<Cell>;
	bool update(protocol::Response::Reader response);

	const Grid& getGrid() const;
	const std::vector<Unit>& getUnits() const;
	const std::vector<Enemy>& getEnemies() const;
	const std::string& getStatus() const;
	int getTick() const;
	int getLevel() const;
	int getOwns() const;
	int getCoverage() const;
	Direction adjustDirection(int unit_index, Direction dir);

private:
	void colorize();
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
