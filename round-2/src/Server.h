#pragma once

#include <memory>
#include <random>
#include <iostream>

#include "Model.h"
#include "Protocol.h"

namespace evil {

class ServerConnection {
public:
	bool Bind();
	bool Accept();
	std::unique_ptr<capnp::PackedFdMessageReader> Read();
	void Write(std::unique_ptr<capnp::MallocMessageBuilder> message);

	~ServerConnection();
private:
	int sockfd_ = -1;
	int newsockfd_ = -1;
};

class Server {
public:
	void SetSeed(int seed);
	void DrawColumn(int col);
	void SetZergsInside(int count);
	void SetZergsOutside(int count);
	void Run();

private:
	bool AcceptLogin();
	void InitModel(int enemies_in, int enemies_out, int border_thickness = 2);
	bool RunGame();
	void ResetRandomEngine();

	// inclusive
	int GetRandom(int low, int high);

	int zergs_inside_ = 7;
	int zergs_outside_ = 0;

	std::tuple<Direction, Direction> GetRandomDirection();

	int draw_col_ = -1;

	ServerConnection connection_;
	Model model_;

	int seed_ = 0;
	std::mt19937 mt_engine_;
};

} // namespace evil
