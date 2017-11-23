#include "Protocol.h"
#include "Connection.h"
#include "Utils.h"
#include "Matrix.h"
#include "Gui.h"
#include "Model.h"
#include "Client.h"
#include "DumbPlayer.h"
#include "Replayer.h"
#include "ZorroPlayer.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <chrono>
#include <thread>
#include <ctime>
#include <iomanip>

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

evil::Model login(evil::Connection& conn) {
	auto message = std::make_unique<capnp::MallocMessageBuilder>();
	auto command = message->initRoot<protocol::Command>();
	auto commands = command.initCommands();
	auto login = commands.initLogin();
	login.setTeam("prezident_evil");
	login.setHash("stzq8jm94kf9iyw7353j9semae2sjorjvthakhzw");
	return *conn.communicate(std::move(message));
}

std::future<std::unique_ptr<evil::Model>> sendMoves(
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

void saveMoves(std::ostream& out,
	const evil::Model& model, const evil::Model::Moves& moves)
{
	out << model.getLevel() << " ";
	out << model.getTick() << " ";
	out << moves.size();
	for (auto m : moves) {
		out << " " << m.first << " " << m.second;
	}
	out << "\n";
}

template<typename Duration>
auto asMs(Duration d) {
	return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

std::string expandtime(const char* fmt) {
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::stringstream ss;
	ss << std::put_time(&tm, fmt);
	return ss.str();
}

int main(int argc, char* argv[]) {
	namespace po = boost::program_options;

	bool stepping = false;
	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help,h", "Print help")
	    ("host,H", po::value<std::string>()->default_value("epb2017.dyndns.org"), "Server host")
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

	evil::Replayer replayer;
	bool use_replay = false;
	if (vm.count("commands")) {
		auto filename = vm["commands"].as<std::string>();
		std::ifstream infile(filename);
		if (infile.fail()) {
			std::cerr << "error: cannot open " << filename << std::endl;
			return 1;
		}
		replayer.load(infile);
		use_replay = true;
	}

	auto host = vm["host"].as<std::string>();
	std::string prefix = (host == "localhost" ? "local_" : "remote_");
	auto filename = "saves/" + prefix + expandtime("%m%d_%H%M%S") + ".txt";
	std::ofstream outfile(filename);

	if (outfile.fail()) {
		std::cerr << "error: cannot write savefile: " << filename << std::endl;
		return 1;
	}

	evil::Connection connection;
	if (!connection.connect(host.c_str(), vm["port"].as<int>())) {
		return 1;
	}

	evil::Gui gui;
	evil::DumbPlayer dumb(&gui);
	gui.init(stepping);

	evil::ZorroPlayer zorro;

	auto model = login(connection);
	auto model_ready = true;
	auto steps_ready = false;

	if (!model.isValid()) {
		std::cerr << "Login failed: " << model.getStatus() << std::endl;
		return 1;
	}

	auto player_string = vm["player"].as<std::string>();
	evil::Player* player = nullptr;
	if (use_replay) {
		player = &replayer;
	} else if (player_string == "gui") {
		player = &gui.getPlayer();
	} else if (player_string == "dumb") {
		player = &dumb;
	} else if (player_string == "zorro") {
		player = &zorro;
	}

	if (!player) {
		std::cerr << "Unknown player " << player_string << std::endl;
		return 1;
	}

	using Clock = std::chrono::steady_clock;

	std::future<std::unique_ptr<evil::Model>> future;
	auto calc_start = Clock::now();
	auto calc_end = Clock::now();
	auto calc_time = asMs(calc_end - calc_start);

	struct OnExit {
		std::string msg;
		OnExit(const std::string& msg) : msg(msg) {}
		~OnExit() {
			std::cerr << msg << std::endl;
		}
	} on_exit("Saved moves to " + filename);

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
			std::cerr << model.getTickInfo()
				<< " - net " << asMs(calc_start - calc_end)
				<< "ms calc " << calc_time << "ms" << std::endl;
		}

		if (!steps_ready && player->isReady()) {
			steps_ready = true;
			auto moves = player->getMoves();
			saveMoves(outfile, model, moves);
			model.provision(moves);

			calc_end = Clock::now();
			calc_time = asMs(calc_end - calc_start);
			future = sendMoves(connection, moves);
		}

		if (evil::isFutureReady(future)) {
			auto model_ptr = future.get();
			if (!model_ptr) {
				return 0;
			}
			model = *model_ptr;
			calc_start = Clock::now();
			model_ready = true;
		} else {
			std::this_thread::yield();
		}
	}

	return 0;
}
