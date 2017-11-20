#pragma once

#include "Trap.h"
#include "Model.h"

namespace evil {

class Fragment {
public:
	virtual ~Fragment() = default;
	virtual Direction getNext(const Model& model) = 0;
	virtual bool isFinished() const = 0;
};

class Converge : public Fragment {
public:
	// Note: this does not work well if target is set to the unit's
	// current position.
	Converge(const Pos& target);
	virtual Direction getNext(const Model& model) override;
	virtual bool isFinished() const override;

private:
	Pos target_;
	bool is_finished_ = false;
};

class Librate : public Fragment {
public:
	virtual Direction getNext(const Model& model) override;
	virtual bool isFinished() const override;
};

class Router : public Fragment {
public:
	void add(Direction dir);
	void add(const std::vector<Direction>& dirs);
	virtual Direction getNext(const Model& model) override;
	virtual bool isFinished() const override;

private:
	std::deque<Direction> dirs_;
};

class Sequence : public Fragment {
public:
	void add(std::unique_ptr<Fragment> fragment);
	virtual Direction getNext(const Model& model) override;
	virtual bool isFinished() const override;

private:
	Direction getNextInternal(const Model& model);

	std::deque<std::unique_ptr<Fragment>> fragments_;
};

class Capture : public Fragment {
public:
	Capture(const Pos& origin, const Trap& trap);
	virtual Direction getNext(const Model& model) override;
	virtual bool isFinished() const override;

private:
	bool isTriggered(const Model& model) const;

	enum class Status {
		kConverge,
		kBuild,
		kWait,
		kSnap,
		kDone
	} status_ = Status::kConverge;

	Pos origin_;
	Pos trigger1_;
	Pos trigger2_;
	Pos snap_origin_;
	Trap trap_;
	std::unique_ptr<Fragment> fragment_;
};

class Spike : public Fragment {
public:
	Spike(const Pos& origin, Direction dir);
	virtual Direction getNext(const Model& model) override;
	virtual bool isFinished() const override;

	static std::vector<Pos> render(const Pos& origin, Direction dir);

private:
	Pos origin_;
	Sequence seq_;
};

class Diagonal : public Fragment {
public:
	Diagonal(const Pos& origin, Direction dir);
	virtual Direction getNext(const Model& model) override;
	virtual bool isFinished() const override;

	static std::vector<Pos> render(const Pos& origin, Direction dir);

private:
	Pos origin_;
	Sequence seq_;
};

class SafeRouter : public Fragment {
public:
	SafeRouter() = default;
	SafeRouter(const Model& model, std::vector<Direction> directions, int unit_idx = 0);

	virtual Direction getNext(const Model& model) override;
	virtual bool isFinished() const override;
private:

	bool LastIsBlue(const Model& model) const;
	bool CanGoFast(const Model& model) const;

	int unit_idx_ = 0;
	int next_direction_idx_ = 1;
	bool can_go_fast_ = false;
	Pos last_pos_;
	std::vector<Direction> directions_;
};

} // namespace evil
