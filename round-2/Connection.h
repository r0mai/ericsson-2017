#pragma once

#include "protocol/Command.capnp.h"
#include "protocol/Response.capnp.h"

#include <capnp/serialize-packed.h>

namespace evil {

protocol::Response::Reader Communicate(capnp::MallocMessageBuilder& message);

} // namespace evil
