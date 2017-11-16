#pragma once
#include <future>
#include <memory>
#include "Protocol.h"


namespace evil {

class Connection {
public:
	bool connect();
	std::unique_ptr<capnp::StreamFdMessageReader> communicate(
		std::unique_ptr<capnp::MallocMessageBuilder> message);

	std::future<std::unique_ptr<capnp::StreamFdMessageReader>> communicateAsync(
		std::unique_ptr<capnp::MallocMessageBuilder> message);

private:
	const char* host_ = "ecovpn.dyndns.org";
	const int port_ = 11224;

	int sockfd_ = -1;
};


} // namespace evil
