#pragma once
#include "Model.h"
#include <vector>
#include <utility>


namespace evil {

class Player {
public:
	virtual ~Player() = default;
	virtual void update(const Model& model) = 0;
	virtual bool isReady() const = 0;
	virtual Model::Moves getMoves() const = 0;
};

} // namespace evil
