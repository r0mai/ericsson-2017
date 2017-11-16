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

struct Unit {
	sf::Vector2i pos;
	sf::Vector2i dir;
};

struct Enemy {
	sf::Vector2i pos;
	sf::Vector2i dir;
};


class Model {
public:
	using Grid = Matrix<int>;
	void update(protocol::Response::Reader response);;
	void dump();
	const Grid& getGrid();

private:
	Grid grid_ {80, 100};
	std::vector<Unit> units_;
	std::vector<Enemy> enemies_;
};

} // namespace evil
