#include "Protocol.h"
#include "Connection.h"
#include "Utils.h"
#include "Matrix.h"
#include "Gui.h"
#include "Model.h"
#include "Client.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <chrono>
#include <thread>

protocol::Response::Reader getResponse(
	std::unique_ptr<capnp::MallocMessageBuilder>& reader)
{
	return reader->getRoot<protocol::Response>();
}

protocol::Direction toProtocol(evil::Direction dir) {
	switch (dir) {
		case evil::Direction::kUp: return protocol::Direction::UP;
		case evil::Direction::kDown: return protocol::Direction::DOWN;
		case evil::Direction::kLeft: return protocol::Direction::LEFT;
		case evil::Direction::kRight: return protocol::Direction::RIGHT;
		case evil::Direction::kNone: return protocol::Direction(-1);
	}
	return protocol::Direction(-1);
}

evil::Direction toDirection(char ch) {
	switch (ch) {
		case '>': return evil::Direction::kRight;
		case '<': return evil::Direction::kLeft;
		case '^': return evil::Direction::kUp;
		case 'v': return evil::Direction::kDown;
		default: break;
	}
	return evil::Direction::kNone;
}

evil::Model login(evil::Connection& conn) {
	auto message = std::make_unique<capnp::MallocMessageBuilder>();
	auto command = message->initRoot<protocol::Command>();
	auto commands = command.initCommands();
	auto login = commands.initLogin();
	login.setTeam("prezident_evil");
	login.setHash("stzq8jm94kf9iyw7353j9semae2sjorjvthakhzw");
	auto reader = conn.communicate(std::move(message));
	return evil::Model::fromResponse(getResponse(reader));
}

evil::Connection::FutureMessage sendMoves(
	evil::Connection& conn,	const evil::Model::Moves& moves)
{
	auto msg = std::make_unique<capnp::MallocMessageBuilder>();
	auto msg_command = msg->initRoot<protocol::Command>();
	auto msg_commands = msg_command.initCommands();
	auto msg_moves = msg_commands.initMoves(moves.size());

	for (int i = 0; i < moves.size(); ++i) {
		auto& move = moves[i];
		auto msg_move = msg_moves[i];
		msg_move.setUnit(move.first);
		msg_move.setDirection(toProtocol(move.second));
	}

	return conn.communicateAsync(std::move(msg));
}

struct Command {
	int level = -1;
	int tick = -1;
	evil::Direction dir = evil::Direction::kNone;
};

std::vector<Command> load(const char* filename) {
	std::ifstream fs(filename);
	if (fs.fail()) {
		return {};
	}

	std::string line;
	std::vector<Command> result;
	while (std::getline(fs, line)) {
		std::stringstream ss(line);
		Command cmd;
		char ch;
		ss >> cmd.level;
		ss >> cmd.tick;
		ss >> ch;
		cmd.dir = toDirection(ch);
		if (cmd.dir == evil::Direction::kNone) {
			break;
		}
		result.push_back(cmd);
	}

	std::cerr
		<< "Loaded " << result.size()
		<< " commands from " << filename << std::endl;
	return result;
}

int main(int argc, char* argv[]) {
	evil::Connection connection;
	int save_arg_index = 1;

	const char* host = "ecovpn.dyndns.org";
	int port = 11224;

	if (argc > 1) {
		if (argv[1] == std::string("local")) {
			save_arg_index = 2;
			host = "localhost";
		}
	}

	// Autoplay
	std::vector<Command> cmds;
	if (argc >= save_arg_index) {
		cmds = load(argv[save_arg_index]);
	}
	auto cmd_it = begin(cmds);
	auto cmd_next = evil::Direction::kNone;

	if (!connection.connect(host, port)) {
		return 1;
	}

	evil::Gui gui;
	gui.init();

	auto model = login(connection);
	auto model_ready = true;
	auto steps_ready = false;

	if (!model.isValid()) {
		std::cerr << "Login failed: " << model.getStatus() << std::endl;
		return 1;
	}

	std::future<evil::Connection::Message> future;

	while (true) {
		// gui phase
		gui.setDrawModel(model);
		if (!gui.update()) {
			break;
		}
		gui.draw();

		// player phase
		auto& player = gui.getPlayer();
		if (model_ready) {
			model_ready = false;
			steps_ready = false;
			player.update(model);
			model.dumpStatus(std::cerr);
		}

		if (!steps_ready && player.isReady()) {
			steps_ready = true;
			auto moves = player.getMoves();
			model.provision(moves);
			future = sendMoves(connection, moves);
		}

		if (evil::isFutureReady(future)) {
			auto reader = future.get();
			if (!reader) {
				return 0;
			}
			auto response = getResponse(reader);
			model = evil::Model::fromResponse(response);
			model_ready = true;
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(8));
		}
	}

	return 0;
}
