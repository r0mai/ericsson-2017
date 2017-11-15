#include <capnp/serialize-packed.h>

#include "protocol/Command.capnp.h"
#include "Connection.h"

int main() {
    capnp::MallocMessageBuilder message;
    auto command = message.initRoot<protocol::Command>();
    auto commands = command.initCommands();
    auto login = commands.initLogin();
    login.setTeam("prezident_evil");
    login.setHash("stzq8jm94kf9iyw7353j9semae2sjorjvthakhzw");
    evil::Communicate(message);
}
