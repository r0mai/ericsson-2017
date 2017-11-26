#include "ZorroStrategyManager.h"

#include <fstream>
#include <cassert>

#include "ZorroStrategyA.h"
#include "ZorroStrategyB.h"
#include "ZorroStrategyC.h"

namespace evil {

namespace {

ZorroStrategyA strategy_A;
ZorroStrategyB strategy_B;
ZorroStrategyC strategy_C;

struct Strategy {
	ZorroStrategy* strategy;
	int skip_ticks_at_start;
};

std::vector<Strategy> strategy_varioations{
	Strategy{&strategy_C, 0},
	Strategy{&strategy_C, 60},
	Strategy{&strategy_C, 120},
	Strategy{&strategy_C, 180},
	//
	Strategy{&strategy_A, 0},
	Strategy{&strategy_B, 0},
	Strategy{&strategy_A, 120},
	Strategy{&strategy_B, 120},
	Strategy{&strategy_A, 180},
	Strategy{&strategy_B, 180},
	Strategy{&strategy_A, 240},
	Strategy{&strategy_B, 240}
};

}

bool ZorroStrategyManager::load(const std::string& filename) {
	std::ifstream infile(filename);
	if (infile.fail()) {
		return false;
	}

	int levels_no;
	infile >> levels_no;

	level_strategies_.resize(levels_no);
	for (int i = 0; i < level_strategies_.size(); ++i) {
		infile >> level_strategies_[i];
	}

	if (!level_strategies_.empty()) {
		auto& last = level_strategies_.back();
		if (last == 1) {
			last = 0;
		} else if (last == 0) {
			last = 2;
		} else {
			++last;
		}
	}

	return true;
}

bool ZorroStrategyManager::save(const std::string& filename) {
	if (levels_asked_ != level_strategies_.size()) {
		return false;
	}

	std::ofstream outfile(filename);
	if (outfile.fail()) {
		return false;
	}

	outfile << level_strategies_.size() << std::endl;
	for (int i = 0; i < level_strategies_.size(); ++i) {
		outfile << level_strategies_[i] << std::endl;
	}

	return true;
}

int ZorroStrategyManager::getSkipTicksNo(int level) {
	return strategy_varioations[getStrategyIndexForLevel(level)].skip_ticks_at_start;
}

ZorroStrategy* ZorroStrategyManager::getStrategy(int level) {
	assert(level == levels_asked_);
	++levels_asked_;
	return strategy_varioations[getStrategyIndexForLevel(level)].strategy;
}

int ZorroStrategyManager::getStrategyIndexForLevel(int level) {
	assert(level <= level_strategies_.size());
	if (level == level_strategies_.size()) {
		level_strategies_.push_back(0);
	}
	return level_strategies_[level];
}

} // namespace evil
