#include "Protocol.h"
#include "Connection.h"
#include "Utils.h"
#include "Matrix.h"
#include "Gui.h"
#include "Model.h"
#include "Client.h"
#include "DumbPlayer.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <chrono>
#include <thread>

#include <boost/program_options.hpp>

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

template<typename Duration>
auto asMs(Duration d) {
	return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

int main(int argc, char* argv[]) {
	namespace po = boost::program_options;

	bool stepping = false;
	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help,h", "Print help")
	    ("host,H", po::value<std::string>()->default_value("ecovpn.dyndns.org"), "Server host")
	    ("port,p", po::value<int>()->default_value(11224), "Server port")
		("player,P", po::value<std::string>()->default_value("gui"), "Controller player")
		("commands,c", po::value<std::string>(), "Commands file")
		("stepping,s", po::bool_switch(&stepping), "Start in stepping mode")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
	    return 1;
	}

	std::vector<Command> cmds;
	if (vm.count("commands")) {
		cmds = load(vm["commands"].as<std::string>().c_str());
	}

	// Autoplay
	auto cmd_it = begin(cmds);
	auto cmd_next = evil::Direction::kNone;

	evil::Connection connection;
	if (!connection.connect(vm["host"].as<std::string>().c_str(), vm["port"].as<int>())) {
		return 1;
	}

	evil::Gui gui;
	evil::DumbPlayer dumb;
	gui.init(stepping);

	auto model = login(connection);
	auto model_ready = true;
	auto steps_ready = false;

	if (!model.isValid()) {
		std::cerr << "Login failed: " << model.getStatus() << std::endl;
		return 1;
	}

	auto player_string = vm["player"].as<std::string>();
	evil::Player* player = nullptr;
	if (player_string == "gui") {
		player = &gui.getPlayer();
	} else if (player_string == "dumb") {
		player = &dumb;
	}

	if (!player) {
		std::cerr << "Unknown player " << player_string << std::endl;
		return 1;
	}

	using Clock = std::chrono::steady_clock;

	std::future<evil::Connection::Message> future;
	auto calc_start = Clock::now();
	auto calc_end = Clock::now();

	while (true) {
		// gui phase
		gui.setDrawModel(model);
		if (!gui.update()) {
			break;
		}
		gui.draw();

		// player phase
		if (model_ready) {
			model_ready = false;
			steps_ready = false;
			player->update(model);
			model.dumpStatus(std::cerr);
		}

		if (!steps_ready && player->isReady()) {
			steps_ready = true;
			auto moves = player->getMoves();
			model.provision(moves);

			calc_end = Clock::now();
			// std::cout << "Calculation time = " << asMs(calc_end - calc_start) << "ms" << std::endl;
			future = sendMoves(connection, moves);
		}

		if (evil::isFutureReady(future)) {
			auto reader = future.get();
			if (!reader) {
				return 0;
			}
			auto response = getResponse(reader);
			model = evil::Model::fromResponse(response);
			calc_start = Clock::now();
			// std::cout << "Network time = " << asMs(calc_start - calc_end) << "ms" << std::endl;
			model_ready = true;
		} else {
			std::this_thread::yield();
		}
	}

	return 0;
}
