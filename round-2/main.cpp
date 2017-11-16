#include "Protocol.h"
#include "Connection.h"
#include "Utils.h"
#include "Matrix.h"
#include "gui.h"
#include "Model.h"
#include "Client.h"
#include <iostream>
#include <memory>


protocol::Response::Reader getResponse(
	std::unique_ptr<capnp::StreamFdMessageReader>& reader)
{
	return reader->getRoot<protocol::Response>();
}


void show(std::unique_ptr<capnp::StreamFdMessageReader> reader) {
	auto response = reader->getRoot<protocol::Response>();

	int row = 0;
	Matrix<int> mat(80, 100);
	for (auto cells : response.getCells()) {
		int col = 0;
		for (auto cell : cells) {
			mat(row, col) = cell.getOwner();
			++col;
		}
		++row;
	}

	for (auto enemy : response.getEnemies()) {
		auto pos = enemy.getPosition();
		mat(pos.getRow(), pos.getCol()) = 2;
	}

	for (auto unit : response.getUnits()) {
		auto pos = unit.getPosition();
		mat(pos.getRow(), pos.getCol()) = 3;
	}

	std::cout << mat << std::endl;

	std::cout << "TICK " << response.getInfo().getTick();
	std::cout << " | owns = " << response.getInfo().getOwns();
	std::cout << " | level = " << response.getInfo().getLevel() << std::endl;
}

int main() {
	evil::Connection connection;
	evil::Model model;
	connection.Connect();
	evil::Gui gui;
	gui.Init();

	{
		auto message = std::make_unique<capnp::MallocMessageBuilder>();
		auto command = message->initRoot<protocol::Command>();
		auto commands = command.initCommands();
		auto login = commands.initLogin();
		login.setTeam("prezident_evil");
		login.setHash("stzq8jm94kf9iyw7353j9semae2sjorjvthakhzw");
		auto reader = connection.Communicate(std::move(message));
		model.update(getResponse(reader));
	}

	auto message = std::make_unique<capnp::MallocMessageBuilder>();
	auto command = message->initRoot<protocol::Command>();
	auto future = connection.CommunicateAsync(std::move(message));

	while (true) {

		if (!gui.Update()) {
			break;
		}

		if (IsFutureReady(future)) {
			auto reader = future.get();
			model.update(getResponse(reader));
			gui.SetModel(model);
			auto message = std::make_unique<capnp::MallocMessageBuilder>();
			auto command = message->initRoot<protocol::Command>();
			auto commands = command.initCommands();
			auto moves = commands.initMoves(1);
			auto move = moves[0];
			move.setDirection(gui.GetLastDirection());
			move.setUnit(0);
			future = connection.CommunicateAsync(std::move(message));
			if (!model.getStatus().empty()) {
				std::cerr << "Status (" <<
					model.getLevel() << ":" << model.getTick() <<
					", " << model.getOwns() << "): " << model.getStatus() << std::endl;
			}
		}
		gui.Draw();
	}

#if 0
	while (true) {
		auto message = step(model);
		auto reader = connection.Communicate(std::move(message));
		model.update(getResponse(reader));
		model.dump();
	}
#endif
}
