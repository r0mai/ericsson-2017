#pragma once

#include "AABB.h"
#include "Player.h"
#include "Fragment.h"

namespace evil {

class DumbPlayer : public Player {
public:
	virtual void update(const Model& model) override;
	virtual bool isReady() const override;
	virtual Model::Moves getMoves() override;

private:
	AABB FindBestArea(const Pos& unit_pos) const;
	void FindBestCut(const Pos& unit_pos);
	std::vector<EnemyState> GetEnemiesInArea(const AABB& aabb) const;

	enum class State {
		kNewMap,
		kNewCut,
		kGoTowardsCutStart,
		kCut
	};

	// state for cut
	struct Cut {
		AABB aabb;
		Pos start;
		Pos end;
		Direction direction;
		SafeRouter router;
	} cut_;

	Model model_;
	State state_;
};

} // namespace evil
