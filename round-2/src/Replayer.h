#pragma once
#include "Player.h"
#include "Model.h"
#include <map>


namespace evil {

class Replayer : public Player {
public:
	void load(std::istream& input);
	void addMoves(int level, int tick, const Model::Moves& moves);

	virtual void update(const Model& model) override;
	virtual bool isReady() const override;
	virtual Model::Moves getMoves() override;

private:
	using Key = std::pair<int, int>;
	using Commands = std::map<Key, Model::Moves>;

	Model model_;
	Commands commands_;
};

} // namespace evil
