#include "Connection.h"

#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>

#include <SFML/Network.hpp>


namespace evil {

bool Connection::connect(const char* host, int port) {
	sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd_ < 0) {
		perror("socket() failed");
		return false;
	}

	int enable = 1;
	if (setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(int)) < 0) {
	    perror("setsockopt(SO_REUSEADDR) failed");
	}

	auto* server = gethostbyname(host);
	if (!server) {
		perror("gethostbyname() failed");
		return false;
	}

	sockaddr_in serv_addr{};
	serv_addr.sin_family = AF_INET;

	bcopy((char *)server->h_addr,
		 (char *)&serv_addr.sin_addr.s_addr,
		 server->h_length);

	serv_addr.sin_port = htons(port);

	if (::connect(sockfd_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect() failed");
		return false;
	}
	return true;
}

void readPackedMessageCopyFromFd(int fd, capnp::MessageBuilder& target,
                           capnp::ReaderOptions options,
						   kj::ArrayPtr<capnp::word> scratchSpace)
{
  kj::FdInputStream stream(fd);
  readMessageCopy(stream, target, options, scratchSpace);
}

std::unique_ptr<Model> Connection::communicate(
	std::unique_ptr<capnp::MallocMessageBuilder> message)
{
	auto response = std::make_unique<capnp::MallocMessageBuilder>();
	capnp::writePackedMessageToFd(sockfd_, *message);
	try {
		// capnp::readMessageCopyFromFd(sockfd_, *response);
		capnp::PackedFdMessageReader reader(sockfd_);
		auto response = reader.getRoot<protocol::Response>();
		return std::make_unique<Model>(
			evil::Model::fromResponse(protocol::Response::Reader{response}));
	} catch(kj::Exception& ex) {
		std::cerr << "Remote closed connection." << std::endl;
		std::cerr << "> " << ex.getDescription().cStr() << std::endl;
		return {};
	}
}

std::future<std::unique_ptr<Model>> Connection::communicateAsync(
	std::unique_ptr<capnp::MallocMessageBuilder> message)
{
	return std::async([this, m = std::move(message)]() mutable {
		return communicate(std::move(m));
	});
}

} // namespace evil
