#pragma once

#include "Player.h"

namespace evil {

class DumbPlayer : public Player {
public:
	virtual void update(const Model& model) override;
	virtual bool isReady() const override;
	virtual Model::Moves getMoves() override;

private:
	enum class State {
		kNewMap
	};

	Model model_;
	State state_;
};

} // namespace evil
