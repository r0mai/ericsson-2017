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
	struct Cut {
		int zigzag_tick = 0;
		Pos start;
		Pos end;
		Direction direction;
	} cut_;

	Model model_;
	State state_;
};

} // namespace evil
