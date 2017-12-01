#include "Server.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <memory>
#include <iostream>

#include "Utils.h"
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

std::unique_ptr<capnp::PackedFdMessageReader> ServerConnection::Read() {
	return std::make_unique<capnp::PackedFdMessageReader>(newsockfd_);
}

void ServerConnection::Write(std::unique_ptr<capnp::MallocMessageBuilder> message) {
	capnp::writePackedMessageToFd(newsockfd_, *message);
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

void Server::SetSeed(int seed) {
	seed_ = seed;
	mt_engine_.seed(seed_);
}

void Server::ResetRandomEngine() {
	if (seed_ != 0) {
		mt_engine_.seed(seed_);
	}
}

void Server::DrawColumn(int col) {
	draw_col_ = col;
}

void Server::SetZergsInside(int count) {
	zergs_inside_ = count;
}

void Server::SetZergsOutside(int count) {
	zergs_outside_ = count;
}

int Server::GetRandom(int low, int high) {
	return getRandom(mt_engine_, low, high);
}

std::tuple<Direction, Direction> Server::GetRandomDirection() {
	Direction v;
	Direction h;

	int direction = GetRandom(0, 3);
	switch (direction) {
		case 0: v = Direction::kUp;   h = Direction::kRight; break;
		case 1: v = Direction::kDown; h = Direction::kRight; break;
		case 2: v = Direction::kDown; h = Direction::kLeft;  break;
		case 3: v = Direction::kUp;   h = Direction::kLeft;  break;
	}

	return std::make_tuple(v, h);
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

			ResetRandomEngine();
			InitModel(zergs_inside_, zergs_outside_);

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

void Server::InitModel(int enemies_in, int enemies_out, int border_thickness) {
	model_ = {};

	model_.setTick(1);
	model_.setLevel(0);

	model_.addStartingPoint(1, {10, 10});
	model_.addStartingPoint(2, {70, 10});
	model_.addStartingPoint(3, {70, 70});
	model_.addStartingPoint(4, {10, 70});

	return;
}

bool Server::RunGame() {
	bool model_invalid = false;
	while (true) {
		connection_.Write(model_.toCapnp());
		if (model_invalid) {
			return false;
		}
		auto reader = connection_.Read();
		auto cmd = reader->getRoot<protocol::Command>();

		if (!cmd.getCommands().isMoves()) {
			std::cerr << "Login received. Expected Moves" << std::endl;
			break;
		}

		for (auto c : cmd.getCommands().getMoves()) {
			auto& units = model_.getAllUnits();
			if (c.getUnit() < units.size()) {
				units[c.getUnit()].dir = fromDirection(c.getDirection());
			} else {
				std::cerr << "Invalid unit idx" << std::endl;
			}
		}

		if (!model_.stepAsServer(mt_engine_)) {
			model_invalid = true;
		}
	}

	return false;
}

} // namespace evil
