#pragma once

#include "protocol/Command.capnp.h"
#include "protocol/Response.capnp.h"

#include <capnp/serialize-packed.h>

namespace evil {

class Connection {
public:
    bool Connect();
    protocol::Response::Reader Communicate(capnp::MallocMessageBuilder& message);

private:
    const char* host = "ecovpn.dyndns.org";
    const int port = 11224;

    int sockfd = -1;
};


} // namespace evil
