#pragma once

#include "AABB.h"
#include "Player.h"

namespace evil {

class DumbPlayer : public Player {
public:
	virtual void update(const Model& model) override;
	virtual bool isReady() const override;
	virtual Model::Moves getMoves() override;

private:
	AABB FindBestArea(const Pos& unit_pos) const;
	void FindBestCut(const Pos& unit_pos);
	bool IsSafe(Pos pos) const;
	bool CanGoFast(const Unit& unit) const;
	int NumberOfEnemiesInArea(const Matrix<bool>& area) const;

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
		bool can_go_fast = false;
	} cut_;

	Model model_;
	State state_;
};

} // namespace evil
