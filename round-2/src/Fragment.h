#pragma once

#include "Trap.h"
#include "Model.h"
#include <memory>


namespace evil {

class Fragment {
public:
	virtual ~Fragment() = default;

	/**
	 * Called once, just before the first `getNext()`, with the same `Model`.
	 * Returns true :the Fragment is not finished.
	 */
	virtual bool init(const Model& model) = 0;

	/**
	 * Can be called any time. The fragment will be discarded after
	 * it is finished.
	 */
	virtual bool isFinished() const = 0;

	/**
	 * Called multiple times, until `isFinished()` is true.
	 * It should only return `Direction::kNone` after the Fragment is finished.
	 */
	virtual Direction getNext(const Model& model) = 0;
};

using Condition = std::function<bool(const Model& model)>;
using FragmentPtr = std::unique_ptr<Fragment>;


class Converge : public Fragment {
public:
	// Note: this does not work well if target is set to the unit's
	// current position.
	Converge(const Pos& target);
	Converge(int unit_idx, const Pos& target);
	virtual bool init(const Model& model) override;
	virtual bool isFinished() const override;
	virtual Direction getNext(const Model& model) override;

private:
	Pos target_;
	int unit_idx_ = 0;
	bool is_finished_ = false;
};


class Librate : public Fragment {
public:
	virtual bool init(const Model& model) override;
	virtual bool isFinished() const override;
	virtual Direction getNext(const Model& model) override;
};


class LibrateUntil : public Fragment {
public:
	LibrateUntil(Condition cond);
	virtual bool init(const Model& model) override;
	virtual bool isFinished() const override;
	virtual Direction getNext(const Model& model) override;

private:
	bool is_finished_ = false;
	Condition cond_;
};


class Router : public Fragment {
public:
	Router() = default;
	Router(const std::vector<Direction>& dirs);
	void add(Direction dir);
	void add(const std::vector<Direction>& dirs);
	virtual bool init(const Model& model) override;
	virtual bool isFinished() const override;
	virtual Direction getNext(const Model& model) override;

private:
	std::deque<Direction> dirs_;
};


class Sequence : public Fragment {
public:
	void add(std::unique_ptr<Fragment> fragment);
	virtual bool init(const Model& model) override;
	virtual bool isFinished() const override;
	virtual Direction getNext(const Model& model) override;

private:
	bool initFirst(const Model& model);
	bool initialized_ = false;
	std::deque<std::unique_ptr<Fragment>> fragments_;
};

class If : public Fragment {
public:
	If(Condition cond, FragmentPtr true_frag, FragmentPtr false_frag);
	virtual bool init(const Model& model) override;
	virtual bool isFinished() const override;
	virtual Direction getNext(const Model& model) override;

private:
	Condition cond_;
	FragmentPtr true_;
	FragmentPtr false_;
};


class Capture : public Fragment {
public:
	Capture(const Pos& bounce, const Alignment& cage_alignment);
	virtual bool init(const Model& model) override;
	virtual bool isFinished() const override;
	virtual Direction getNext(const Model& model) override;

private:
	bool isTriggered(const Model& model) const;

	enum class Phase {
		kPrepare,
		kWait,
		kSnap,
		kDone
	} phase_ = Phase::kPrepare;

	Pos trigger_;
	Sequence seq_;

	Router snap_;
	Librate lib_;

	int wait_ = 0;
	Direction snap_first_ = Direction::kNone;
};


class SafeRouter : public Fragment {
public:
	SafeRouter() = default;
	SafeRouter(std::vector<Direction> directions, int unit_idx = 0);

	virtual bool init(const Model& model) override;
	virtual bool isFinished() const override;
	virtual Direction getNext(const Model& model) override;

private:
	bool lastIsBlue(const Model& model) const;
	bool canGoFast(const Model& model) const;

	int unit_idx_ = 0;
	int next_direction_idx_ = 0;
	bool can_go_fast_ = false;
	Pos last_pos_;
	std::vector<Direction> directions_;
};


class SafeLibrate : public Fragment {
public:
	// target_pos must be blue
	virtual bool init(const Model& model) override;
	virtual bool isFinished() const override;
	virtual Direction getNext(const Model& model) override;
private:

	Pos target_pos_;
};

} // namespace evil
