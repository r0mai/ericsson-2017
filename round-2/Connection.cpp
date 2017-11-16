#include "Connection.h"

#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>

#include <capnp/serialize.h>

#include <SFML/Network.hpp>


namespace evil {

bool Connection::connect() {
	const char* host = host_;
	const int port = port_;

	sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd_ < 0) {
		std::cerr << "socket() failed " << sockfd_ << std::endl;
		return false;
	}

	auto* server = gethostbyname(host);
	if (!server) {
		std::cerr << "gethostbyname() failed" << std::endl;
		return false;
	}

	sockaddr_in serv_addr{};
	serv_addr.sin_family = AF_INET;

	bcopy((char *)server->h_addr,
		 (char *)&serv_addr.sin_addr.s_addr,
		 server->h_length);

	serv_addr.sin_port = htons(port);

	if (::connect(sockfd_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		std::cerr << "connect() failed " << std::endl;
		return false;
	}
	return true;
}

std::unique_ptr<capnp::StreamFdMessageReader> Connection::communicate(
	std::unique_ptr<capnp::MallocMessageBuilder> message)
{
	capnp::writeMessageToFd(sockfd_, *message);

	return std::make_unique<capnp::StreamFdMessageReader>(sockfd_);
}

std::future<std::unique_ptr<capnp::StreamFdMessageReader>> Connection::communicateAsync(
	std::unique_ptr<capnp::MallocMessageBuilder> message)
{
	return std::async([this, m = std::move(message)]() mutable {
		return communicate(std::move(m));
	});
}

} // namespace evil
