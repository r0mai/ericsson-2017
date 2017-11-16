#pragma once
#include "Matrix.h"
#include "Protocol.h"
#include <SFML/System/Vector2.hpp>
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
	bool is_unit = false;
};

class Model {
public:
	using Grid = Matrix<Cell>;
	void update(protocol::Response::Reader response);;
	void dump();
	const Grid& getGrid() const;
	const std::vector<Unit>& getUnits() const;
	const std::vector<Enemy>& getEnemies() const;

private:
	Grid grid_ {80, 100};
	std::vector<Unit> units_;
	std::vector<Enemy> enemies_;
};

} // namespace evil
