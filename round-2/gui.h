#pragma once

#include <SFML/Window/Event.hpp>
#include "protocol/Response.capnp.h"

namespace evil {
namespace gui {

bool Init();
void DrawGameState(protocol::Response::Reader& response_reader);
bool PollEvent(sf::Event& event);

}
}
