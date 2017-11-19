#pragma once

#include "Player.h"

namespace evil {

class DumbPlayer : public Player {
public:
	virtual void update(const Model& model) override;
	virtual bool isReady() const override;
	virtual Model::Moves getMoves() override;

private:
	std::pair<Pos, Pos> FindBiggestArea() const;
	void FindBestCut();
	bool CheckIfSafe(Pos pos) const;

	enum class State {
		kNewMap,
		kNewCut,
		kGoTowardsCutStart,
		kCut
	};

	// state for cut
	int cut_zigzag_tick_ = 0;
	Pos cut_start_;
	Pos cut_end_;
	Direction cut_direction_;

	Model model_;
	State state_;
};

} // namespace evil
