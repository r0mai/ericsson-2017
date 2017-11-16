#include "Model.h"


namespace evil {
namespace {

Pos fromDirection(protocol::Direction dir) {
    Pos pos;
    switch(dir) {
        case protocol::Direction::LEFT: pos.col = -1; break;
        case protocol::Direction::RIGHT: pos.col = 1; break;
        case protocol::Direction::UP: pos.row = -1; break;
        case protocol::Direction::DOWN: pos.row = 1; break;
        default: break;
    }

    return pos;
}

} // namespace


Pos& operator+=(Pos& lhs, const Pos& rhs) {
    lhs.row += rhs.row;
    lhs.col += rhs.col;
    return lhs;
}

Pos operator+(const Pos& lhs, const Pos& rhs) {
    Pos pos = lhs;
    pos += rhs;
    return pos;
}

std::ostream& operator<<(std::ostream& out, const Pos& pos) {
    out << "(" << pos.row << ", " << pos.col << ")";
    return out;
}


const Model::Grid& Model::getGrid() const {
	return grid_;
}

const std::vector<Unit>& Model::getUnits() const {
	return units_;
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

void Model::update(protocol::Response::Reader response) {
	auto& mat = grid_;
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
	units_.clear();
	enemies_.clear();
	status_.clear();

    for (auto enemy : response.getEnemies()) {
        auto pos = enemy.getPosition();
		Enemy e;
		e.pos = Pos{pos.getRow(), pos.getCol()};
        e.dir = fromDirection(enemy.getDirection().getVertical()) +
            fromDirection(enemy.getDirection().getHorizontal());
		enemies_.push_back(e);
    }

    for (auto unit : response.getUnits()) {
        auto pos = unit.getPosition();
		Unit u;
		u.pos = Pos{pos.getRow(), pos.getCol()};
        u.dir = fromDirection(unit.getDirection());
		units_.push_back(u);
    }

	status_ = response.getStatus().cStr();
	tick_ = response.getInfo().getTick();
	level_ = response.getInfo().getLevel();
	owns_ = response.getInfo().getOwns();

    colorize();
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
        grid_(pos.row, pos.col).color = 3;
    }

    for (auto& enemy : enemies_) {
        auto pos = enemy.pos;
        auto* cell = &grid_(pos.row, pos.col);
        while (cell->owner != 1) {
            cell->color = 5;
            pos += enemy.dir;
            cell = &grid_(pos.row, pos.col);
        }
    }

    for (auto& enemy : enemies_) {
        auto pos = enemy.pos;
        grid_(pos.row, pos.col).color = 4;
    }
}

} // namespace evil
