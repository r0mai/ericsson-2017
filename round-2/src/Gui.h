#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include <deque>
#include "Protocol.h"
#include "Model.h"
#include "Player.h"
#include "Trap.h"

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
	virtual Direction getNext(const Model& model);
	virtual bool isFinished() const;

private:
	std::deque<std::unique_ptr<Fragment>> fragments_;
};

class Capture : public Fragment {
public:
	Capture(const Pos& origin, const Trap& trap);
	virtual Direction getNext(const Model& model);
	virtual bool isFinished() const;

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


// Gui /////////////////////////////////////////////////////////////////////////

class Gui {
public:
	Gui();

	bool init(bool stepping);
	void close();
	bool update();
	void updateStatus();
	void draw();
	void setDrawModel(Model model);
	bool pollEvent(sf::Event& event);
	Direction getDirection() const;
	Player& getPlayer();

private:
	using Clock = std::chrono::steady_clock;

	static constexpr int map_w = 100;
	static constexpr int map_h = 80;
	static constexpr int window_w = 1024 * 2;
	static constexpr int window_h = 768 * 2;
	static constexpr float cell_w = (float)window_w / map_w;
	static constexpr float cell_h = (float)window_h / map_h;

	class GuiPlayer
		: public Player {
	public:
		GuiPlayer(Gui* gui);
		virtual void update(const Model& model) override;
		virtual bool isReady() const override;
		virtual Model::Moves getMoves() override;

	private:
		Gui* gui_;
	};

	void onPlayerUpdate(const Model& model);
	bool isReady();
	Model::Moves getMoves();

	void drawCell(const Pos& pos, const Cell& cell);
	void drawCell(const Pos& pos, sf::Color color);
	void drawDot(const Pos& pos, sf::Color color, float scale = 1.f);

	Pos windowToPos(int x, int y) const;
	void handleKeypress(const sf::Event::KeyEvent& ev);
	void handleMouseButton(const sf::Event::MouseButtonEvent& ev);

	void toggleLibrate();
	void toggleManual(Direction dir);
	void toggleTrap(const Pos& origin, Direction axis0, Direction axis1);
	void toggleCycle();
	void toggleStepping(bool enable);

	sf::RenderWindow window_ {sf::VideoMode(window_w, window_h), "Window"};
	Model model_;
	Direction dir_ = Direction::kUp;
	Pos mouse_pos_;

	GuiPlayer player_;
	Clock::time_point last_update_;
	int delay_ = 0;
	int cycle_ = 0;
	bool is_stepping_ = false;
	bool step_ready_ = false;

	enum Mode {
		kNormal,
		kTrap,
		kSpike,
		//
		kModeCount
	} mode_ = Mode::kNormal;

	std::unique_ptr<Fragment> fragment_;
};

} // namespace evil
