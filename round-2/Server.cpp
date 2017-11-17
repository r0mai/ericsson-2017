#include "Server.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <memory>
#include <iostream>

#include "Protocol.h"

namespace evil {

bool ServerConnection::Bind() {
	sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd_ < 0) {
		perror("ERROR opening socket");
		return false;
	}

	int enable = 1;
	if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
	    perror("setsockopt(SO_REUSEADDR) failed");
	}

	sockaddr_in serv_addr{};

	int portno = 11224;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd_, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		return false;
	}

	return true;
}

bool ServerConnection::Accept() {
	sockaddr_in cli_addr{};
	listen(sockfd_, 5);
	socklen_t clilen = sizeof(cli_addr);
	newsockfd_ = accept(sockfd_, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd_ < 0) {
		perror("ERROR on accept");
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

bool Server::AcceptLogin() {
	auto msg = connection_.Read();
	auto login_cmd = msg->getRoot<protocol::Command>();

	if (!login_cmd.getCommands().isLogin()) {
		std::cerr << "No login message received" << std::endl;
		return false;
	}

	std::cout << "Successful login of "
		<< login_cmd.getCommands().getLogin().getTeam().cStr() << std::endl;

	return true;
}

void Server::Run() {
	if (!connection_.Bind()) {
		return;
	}

	while (true) {
		try {
			if (!connection_.Accept()) {
				continue;
			}

			if (!AcceptLogin()) {
				continue;
			}

			if (!RunGame()) {
				continue;
			}


		} catch (std::exception& ex) {
			std::cerr << "Exception caught " << ex.what() << std::endl;
		} catch (...) {
			std::cerr << "Unknown exception caught" << std::endl;
			return;
		}
	}
}

bool Server::RunGame() {
	{
		Unit unit;
		unit.health = 6;
		unit.killer = 3;
		unit.owner = 1;
		unit.pos = {0, 0};
		unit.dir = Direction::kDown;

		model_.addUnit(unit);
	};

	model_.addBorder();

	while (true) {
		connection_.Write(model_.toCapnp());
		auto reader = connection_.Read();
		auto cmd = reader->getRoot<protocol::Command>();

		if (!cmd.getCommands().isMoves()) {
			std::cerr << "Login received. Expected Moves" << std::endl;
			break;
		}
	}

	return false;
}

} // namespace evil
