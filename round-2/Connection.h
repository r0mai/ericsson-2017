#pragma once
#include <future>
#include <memory>
#include "Protocol.h"


namespace evil {

class Connection {
public:
	bool connect(const char* host = "ecovpn.dyndns.org", int port = 11224);
	std::unique_ptr<capnp::MallocMessageBuilder> communicate(
		std::unique_ptr<capnp::MallocMessageBuilder> message);

	std::future<std::unique_ptr<capnp::MallocMessageBuilder>> communicateAsync(
		std::unique_ptr<capnp::MallocMessageBuilder> message);

private:
	int sockfd_ = -1;
};


} // namespace evil
