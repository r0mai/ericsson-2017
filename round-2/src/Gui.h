#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include <deque>
#include "Protocol.h"
#include "Model.h"
#include "Player.h"
#include "Trap.h"
#include "Fragment.h"

namespace evil {

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
	void setPlayer(std::unique_ptr<Player> player);
	Direction getDirection() const;
	Player& getPlayer();

	Pos lastClickedPosAndClear();

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
		void setPlayer(std::unique_ptr<Player> player);
		virtual void update(const Model& model) override;
		virtual bool isReady() const override;
		virtual Model::Moves getMoves() override;

	private:
		Gui* gui_;
		std::unique_ptr<Player> player_;
	};

	void onPlayerUpdate(const Model& model);
	bool isReady();
	Model::Moves getMoves();

	void drawCell(const Pos& pos, const Cell& cell);
	void drawCell(const Pos& pos, sf::Color color);
	void drawCells(const std::vector<Pos>& vec, sf::Color color);
	void drawDot(const Pos& pos, sf::Color color, float scale = 1.f);
	void drawDirection(const Pos& pos, Direction h_dir, Direction v_dir);

	Pos windowToPos(int x, int y) const;
	void handleKeypress(const sf::Event::KeyEvent& ev);
	void handleMouseButton(const sf::Event::MouseButtonEvent& ev);

	void setFragment(std::unique_ptr<Fragment> fragment);
	void toggleLibrate();
	void toggleManual(Direction dir);
	void toggleTrap(const Pos& origin, Direction axis0, Direction axis1);
	void toggleCycle();
	void toggleStepping(bool enable);
	void toggleCapture();
	void toggleHardcore();
	void toggleZorroBegin();
	void toggleZorroConquerLeft();
	void toggleZorroConquerRight();

	void toggleZorroBegin2();
	void toggleZorroConquerLeft2();
	void toggleZorroConquerRight2();

	Alignment getAlignment();

	sf::RenderWindow window_ {sf::VideoMode(window_w, window_h), "Window"};
	Model model_;
	Direction dir_ = Direction::kUp;
	Pos mouse_pos_;
	Pos last_clicked_pos_;
	GuiPlayer player_;

	Clock::time_point last_update_;
	int clamp_w_ = 1;
	int diag_w_ = 10;
	int delay_ = 0;
	int cycle_ = 0;
	bool is_stepping_ = false;
	bool step_ready_ = false;
	bool reverse_ = false;

	enum Mode {
		kNormal,
		kDiagonal,
		kClamp,
		kInverse,
		//
		kModeCount
	} mode_ = Mode::kNormal;

	std::unique_ptr<Fragment> fragment_;
};

} // namespace evil
