#include "Player.h"
#include "Fragment.h"

#include <unordered_map>

namespace evil {

class FinalPlayer : public Player {
public:
	virtual void update(const Model& model) override;
	virtual bool isReady() const override;
	virtual Model::Moves getMoves() override;

	void onNewMap();
	void onUnitDied(int unit_idx);

	struct TraversableAABB {
		Pos initial_pos; // on our area
		AABB aabb;
	};

	TraversableAABB getTraversableAABB(
		const Unit& unit, const AABB& other_aabb);

	std::vector<Direction> getPathForAABB(const TraversableAABB& aabb);

private:
	Model model_;
	std::unordered_map<int, std::unique_ptr<Sequence>> fragments_;
};

} // namespace evil
