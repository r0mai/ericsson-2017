#include "Client.h"

namespace evil {

std::unique_ptr<capnp::MallocMessageBuilder> step(const Model& model) {
    auto message = std::make_unique<capnp::MallocMessageBuilder>();
    auto command = message->initRoot<protocol::Command>();
    auto commands = command.initCommands();
    auto moves = commands.initMoves(0);
    return message;
}

} // namespace evil
