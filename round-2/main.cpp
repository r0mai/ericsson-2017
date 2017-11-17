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


protocol::Response::Reader getResponse(
	std::unique_ptr<capnp::StreamFdMessageReader>& reader)
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
	evil::Model model;

	if (!connection.connect()) {
		return 1;
	}

	std::vector<Command> cmds;
	if (argc > 0) {
		cmds = load(argv[1]);
	}

	auto cmd_it = begin(cmds);
	auto cmd_next = evil::Direction::kNone;

	evil::Gui gui;
	gui.init();

	{
		auto message = std::make_unique<capnp::MallocMessageBuilder>();
		auto command = message->initRoot<protocol::Command>();
		auto commands = command.initCommands();
		auto login = commands.initLogin();
		login.setTeam("prezident_evil");
		login.setHash("stzq8jm94kf9iyw7353j9semae2sjorjvthakhzw");
		auto reader = connection.communicate(std::move(message));
		auto response = getResponse(reader);
		if (!model.update(response)) {
			std::cerr << model.getStatus() << std::endl;
			return 0;
		}
	}

	auto message = std::make_unique<capnp::MallocMessageBuilder>();
	auto command = message->initRoot<protocol::Command>();
	auto future = connection.communicateAsync(std::move(message));
	auto prev = evil::Direction::kNone;

	while (true) {
		if (!gui.update()) {
			break;
		}

		if (isFutureReady(future)) {
			auto reader = future.get();
			if (!model.update(getResponse(reader))) {
				std::cerr << model.getStatus() << std::endl;
				gui.close();
				break;
			}
			gui.setModel(model);
			gui.updateStatus();
			auto message = std::make_unique<capnp::MallocMessageBuilder>();
			auto command = message->initRoot<protocol::Command>();
			auto commands = command.initCommands();
			auto moves = commands.initMoves(1);
			auto move = moves[0];

			auto next = gui.getDirection();
			if (next != evil::Direction::kNone) {
				cmd_it = end(cmds);
			}

			if (cmd_it != end(cmds)) {
				auto& cmd = *cmd_it;
				if (cmd.level == model.getLevel() &&
					cmd.tick == model.getTick())
				{
					cmd_next = cmd.dir;
					++cmd_it;
				}
				next = cmd_next;
			}

			next = model.adjustDirection(0, next);
			move.setDirection(toProtocol(next));
			move.setUnit(0);

			if (next != prev) {
				prev = next;
				std::cout
					<< model.getLevel() << " " << model.getTick()
					<< " " << next << std::endl;
			}

			future = connection.communicateAsync(std::move(message));
		}
		gui.draw();
	}
}
