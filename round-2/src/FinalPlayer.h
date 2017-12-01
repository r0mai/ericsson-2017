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

private:
	Model model_;
	std::unordered_map<int, std::unique_ptr<Fragment>> fragments_;
};

} // namespace evil
