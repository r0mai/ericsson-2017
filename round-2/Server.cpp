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

void Server::SetSeed(int seed) {
	mt_engine_.seed(seed);
}

int Server::GetRandom(int low, int high) {
	std::uniform_int_distribution<int> dist(low, high);
	return dist(mt_engine_);
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

			InitModel(3, 0);

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

	model_.addBorder(1, border_thickness);

	{
		Unit unit;
		unit.health = 6;
		unit.killer = 3;
		unit.owner = 1;
		unit.pos = {0, 0};
		unit.dir = Direction::kDown;

		model_.addUnit(unit);
	};

	auto& grid = model_.getGrid();
	auto& enemies = model_.getEnemies();

	for (int i = 0; i < enemies_in; ++i) {
		Enemy enemy;
		enemy.pos.row = GetRandom(border_thickness, grid.rows() - border_thickness - 1);
		enemy.pos.col = GetRandom(border_thickness, grid.cols() - border_thickness - 1);

		int direction = GetRandom(0, 3);
		switch (direction) {
			case 0: enemy.v_dir = Direction::kUp;   enemy.h_dir = Direction::kRight; break;
			case 1: enemy.v_dir = Direction::kDown; enemy.h_dir = Direction::kRight; break;
			case 2: enemy.v_dir = Direction::kDown; enemy.h_dir = Direction::kLeft;  break;
			case 3: enemy.v_dir = Direction::kUp;   enemy.h_dir = Direction::kLeft;  break;
		}

		enemies.push_back(enemy);
	}
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
			auto& units = model_.getUnits();
			if (c.getUnit() < units.size()) {
				units[c.getUnit()].dir = fromDirection(c.getDirection());
			} else {
				std::cerr << "Invalid unit idx" << std::endl;
			}
		}

		if (!model_.stepAsServer()) {
			model_invalid = true;
		}
	}

	return false;
}

} // namespace evil
