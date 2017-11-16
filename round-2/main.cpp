#include <capnp/serialize.h>

#include "protocol/Command.capnp.h"
#include "protocol/Response.capnp.h"
#include "Connection.h"
#include "Utils.h"
#include "Matrix.h"
#include "gui.h"

#include <iostream>
#include <memory>


template<typename T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& mat) {
    for (int row = 0; row < mat.width(); ++row) {
        for (int col = 0; col < mat.height(); ++col) {
            out << mat(row, col);
        }
        out << std::endl;
    }
    return out;
}


void step(std::unique_ptr<capnp::StreamFdMessageReader> reader) {
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

    std::cout << mat << std::endl;

    std::cout << "TICK " << response.getInfo().getTick();
    std::cout << " | owns = " << response.getInfo().getOwns();
    std::cout << " | level = " << response.getInfo().getLevel() << std::endl;

}

int main() {
    evil::Connection connection;
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
        std::cout << reader->getRoot<protocol::Response>().getStatus().cStr() << std::endl;
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
}
