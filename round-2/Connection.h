#pragma once

#include <future>
#include <memory>

#include "protocol/Command.capnp.h"
#include "protocol/Response.capnp.h"

#include <capnp/serialize-packed.h>

namespace evil {

class Connection {
public:
    bool Connect();
    std::unique_ptr<capnp::StreamFdMessageReader> Communicate(
        std::unique_ptr<capnp::MallocMessageBuilder> message);

    std::future<std::unique_ptr<capnp::StreamFdMessageReader>> CommunicateAsync(
        std::unique_ptr<capnp::MallocMessageBuilder> message);

private:
    const char* host = "ecovpn.dyndns.org";
    const int port = 11224;

    int sockfd = -1;
};


} // namespace evil
