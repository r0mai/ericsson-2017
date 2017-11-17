#include "Server.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <memory>
#include <iostream>

#include "Protocol.h"

namespace evil {

bool ServerConnection::Accept() {
	sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd_ < 0) {
		std::cerr << "ERROR opening socket" << std::endl;
		return false;
	}

	sockaddr_in serv_addr{};
	sockaddr_in cli_addr{};

	int portno = 11224;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd_, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		std::cerr << "Error on binding" << std::endl;
		return false;
	}

	listen(sockfd_, 5);
	socklen_t clilen = sizeof(cli_addr);
	newsockfd_ = accept(sockfd_, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd_ < 0) {
		std::cerr << "ERROR on accept" << std::endl;
		return false;
	}

	return true;
}

std::unique_ptr<capnp::StreamFdMessageReader> ServerConnection::Read() {
	return std::make_unique<capnp::StreamFdMessageReader>(newsockfd_);
}

void ServerConnection::Write(std::unique_ptr<capnp::MallocMessageBuilder> message) {
	capnp::writeMessageToFd(newsockfd_, *message);
}

ServerConnection::~ServerConnection() {
	close(sockfd_);
	close(newsockfd_);
}

void Server::Run() {
	if (!connection_.Accept()) {
		return;
	}

	auto msg = connection_.Read();
	std::cout << msg->getRoot<protocol::Command>().toString().flatten().cStr() << std::endl;

}

} // namespace evil
