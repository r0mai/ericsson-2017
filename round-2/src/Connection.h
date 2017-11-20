#pragma once
#include <future>
#include <memory>
#include "Protocol.h"


namespace evil {

class Connection {
public:
	using Message = std::unique_ptr<capnp::MallocMessageBuilder>;
	using FutureMessage = std::future<Message>;

	bool connect(const char* host, int port);
	Message communicate(Message msg);
	FutureMessage communicateAsync(Message msg);

private:
	int sockfd_ = -1;
};


} // namespace evil
