#pragma once

#include <memory>
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
	void Run();

private:
	bool AcceptLogin();

	ServerConnection connection_;
	Model model_;
};

} // namespace evil
