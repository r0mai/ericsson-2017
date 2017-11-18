#pragma once
#include "Model.h"
#include <memory>

namespace evil {

std::unique_ptr<capnp::MallocMessageBuilder> step(const Model& model);

} // namespace evil
