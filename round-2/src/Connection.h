#pragma once
#include <future>
#include <memory>
#include "Protocol.h"
#include "Model.h"

namespace evil {

class Connection {
public:
	bool connect(const char* host, int port);
	std::unique_ptr<Model> communicate(std::unique_ptr<capnp::MallocMessageBuilder> msg);
	std::future<std::unique_ptr<Model>> communicateAsync(
		std::unique_ptr<capnp::MallocMessageBuilder> msg);

private:
	int sockfd_ = -1;
};


} // namespace evil
