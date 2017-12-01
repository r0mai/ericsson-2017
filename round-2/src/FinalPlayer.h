#include "Player.h"
#include "Fragment.h"

namespace evil {

class FinalPlayer : public Player {
public:
	virtual void update(const Model& model) override;
	virtual bool isReady() const override;
	virtual Model::Moves getMoves() override;

private:
	Model model_;
	std::vector<std::unique_ptr<Fragment>> fragments_;
};

} // namespace evil
