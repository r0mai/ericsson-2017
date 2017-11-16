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
	evil::gui::Init();

    {
        auto message = std::make_unique<capnp::MallocMessageBuilder>();
        auto command = message->initRoot<protocol::Command>();
        auto commands = command.initCommands();
        auto login = commands.initLogin();
        login.setTeam("prezident_evil");
        login.setHash("stzq8jm94kf9iyw7353j9semae2sjorjvthakhzw");
        auto reader = connection.Communicate(std::move(message));
        model.update(getResponse(reader));
        model.dump();
    }

	while (true) {
		auto message = std::make_unique<capnp::MallocMessageBuilder>();
		auto command = message->initRoot<protocol::Command>();

		sf::Event event;
		while (evil::gui::PollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
					case sf::Keyboard::Up:
					{
						auto commands = command.initCommands();
						auto moves = commands.initMoves(1);
						auto move = moves[0];
						move.setDirection(protocol::Direction::UP);
						move.setUnit(0);
						break;
					}

					case sf::Keyboard::Down:
					{
						auto commands = command.initCommands();
						auto moves = commands.initMoves(1);
						auto move = moves[0];
						move.setDirection(protocol::Direction::DOWN);
						move.setUnit(0);
						break;
					}

					case sf::Keyboard::Left:
					{
						auto commands = command.initCommands();
						auto moves = commands.initMoves(1);
						auto move = moves[0];
						move.setDirection(protocol::Direction::LEFT);
						move.setUnit(0);
						break;
					}

					case sf::Keyboard::Right:
					{
						auto commands = command.initCommands();
						auto moves = commands.initMoves(1);
						auto move = moves[0];
						move.setDirection(protocol::Direction::RIGHT);
						move.setUnit(0);
						break;
					}
				}
			}

		}

		auto future = connection.CommunicateAsync(std::move(message));
		for (int i = 0; ; ++i) {
			if (IsFutureReady(future)) {
				auto reader = future.get();
				auto response = reader->getRoot<protocol::Response>();
				evil::gui::DrawGameState(response);
				break;
			}
		}
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
