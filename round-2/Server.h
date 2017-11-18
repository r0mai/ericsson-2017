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
	std::unique_ptr<capnp::StreamFdMessageReader> Read();
	void Write(std::unique_ptr<capnp::MallocMessageBuilder> message);

	~ServerConnection();
private:
	int sockfd_ = -1;
	int newsockfd_ = -1;
};

class Server {
public:
	void SetSeed(int seed);
	void Run();

private:
	bool AcceptLogin();
	void InitModel(int enemies_in, int enemies_out, int border_thickness = 2);
	bool RunGame();

	// inclusive
	int GetRandom(int low, int high);

	ServerConnection connection_;
	Model model_;

	std::mt19937 mt_engine_;
};

} // namespace evil
