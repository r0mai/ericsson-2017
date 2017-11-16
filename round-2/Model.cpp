#include "Model.h"


namespace evil {

const Model::Grid& Model::getGrid() const {
	return grid_;
}

const std::vector<Unit>& Model::getUnits() const {
	return units_;
}

const std::vector<Enemy>& Model::getEnemies() const {
	return enemies_;
}

void Model::update(protocol::Response::Reader response) {
	auto& mat = grid_;
    int row = 0;
    for (auto cells : response.getCells()) {
        int col = 0;
        for (auto cell : cells) {
            mat(row, col) = cell.getOwner();
            ++col;
        }
        ++row;
    }
	units_.clear();
	enemies_.clear();

    for (auto enemy : response.getEnemies()) {
        auto pos = enemy.getPosition();
		Enemy e;
		e.pos = Pos{pos.getRow(), pos.getCol()};
		enemies_.push_back(e);
    }

    for (auto unit : response.getUnits()) {
        auto pos = unit.getPosition();
		Unit u;
		u.pos = Pos{pos.getRow(), pos.getCol()};
		units_.push_back(u);
    }
}

void Model::dump() {
    std::cout << grid_ << std::endl;
    // std::cout << "TICK " << response.getInfo().getTick();
    // std::cout << " | owns = " << response.getInfo().getOwns();
    // std::cout << " | level = " << response.getInfo().getLevel() << std::endl;
}

} // namespace evil
