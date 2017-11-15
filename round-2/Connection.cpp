#include "Connection.h"

#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <capnp/serialize-packed.h>

#include <iostream>

#include <SFML/Network.hpp>

namespace evil {

bool Connection::Connect() {
    const char* host = "ecovpn.dyndns.org";
    const int port = 11224;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "socket() failed " << sockfd << std::endl;
        return false;
    }

    auto* server = gethostbyname(host);
    if (!server) {
        std::cerr << "gethostbyname() failed " << std::endl;
        return false;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    serv_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "connect() failed " << std::endl;
        return false;
    }
    return true;
}

protocol::Response::Reader Connection::Communicate(capnp::MallocMessageBuilder& message) {
    capnp::writeMessageToFd(sockfd, message);

    capnp::StreamFdMessageReader reader(sockfd);
    auto response = reader.getRoot<protocol::Response>();
    std::cout << "Response = " << response.getStatus().cStr() << std::endl;

    return {};
}

} // namespace evil
