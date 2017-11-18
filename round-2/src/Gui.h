#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include "Protocol.h"
#include "Model.h"
#include "Player.h"

namespace evil {


class Gui {
public:
	Gui();

	bool init();
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

	enum class Mode {
		kNormal,
		kLibrate,
		kTracking
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
	std::vector<Pos> makeTrap(const Pos& origin);

	sf::RenderWindow window_ {sf::VideoMode(window_w, window_h), "Window"};
	Model model_;
	Direction dir_ = Direction::kNone;
	Pos mouse_pos_;

	GuiPlayer player_;
	Clock::time_point last_update_;
	int delay_ = 0;

	Mode mode_ = Mode::kNormal;
	Direction librate_dir_ = Direction::kNone;
	Pos target_pos_;
};

} // namespace evil
