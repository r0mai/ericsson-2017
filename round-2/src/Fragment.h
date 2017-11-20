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
	Direction forceGetNext(const Model& model);
	virtual Direction getNext(const Model& model) override;
	virtual bool isFinished() const override;

private:
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
	virtual Direction getNext(const Model& model);
	virtual bool isFinished() const;

	static std::vector<Pos> render(const Pos& origin, Direction dir);

private:
	Pos origin_;
	Sequence seq_;
};

class Diagonal : public Fragment {
public:
	Diagonal(const Pos& origin, Direction dir);
	virtual Direction getNext(const Model& model);
	virtual bool isFinished() const;

	static std::vector<Pos> render(const Pos& origin, Direction dir);

private:
	Pos origin_;
	Sequence seq_;
};



} // namespace evil
