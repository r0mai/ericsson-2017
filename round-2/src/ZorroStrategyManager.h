#pragma once

#include <string>
#include <vector>

namespace evil {

class ZorroStrategy;

class ZorroStrategyManager {
public:
	bool load(const std::string& filename);
	bool save(const std::string& filename);

	int getSkipTicksNo(int level);
	ZorroStrategy* getStrategy(int level);
private:
	int getStrategyIndexForLevel(int level);

	std::vector<int> level_strategies_;
	int levels_asked_ = 0;
};

} // namespace evil
