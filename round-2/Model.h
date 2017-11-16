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

struct Unit {
	Pos pos;
	Pos dir;
};

struct Enemy {
	Pos pos;
	Pos dir;
};

struct Cell {
	int owner = 0;
	bool is_attacked = false;

	// algo
	int color = 0;
};


Pos& operator+=(Pos& lhs, const Pos& rhs);
Pos operator+(const Pos& lhs, const Pos& rhs);
std::ostream& operator<<(std::ostream& out, const Pos& pos);


class Model {
public:
	using Grid = Matrix<Cell>;
	void update(protocol::Response::Reader response);;

	const Grid& getGrid() const;
	const std::vector<Unit>& getUnits() const;
	const std::vector<Enemy>& getEnemies() const;
	const std::string& getStatus() const;
	int getTick() const;
	int getLevel() const;
	int getOwns() const;

private:
	void colorize();

	Grid grid_ {80, 100};
	std::vector<Unit> units_;
	std::vector<Enemy> enemies_;

	std::string status_;
	int tick_ = -1;
	int level_ = -1;
	int owns_ = -1;
};

} // namespace evil
