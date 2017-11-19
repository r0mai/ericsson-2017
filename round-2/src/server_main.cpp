#include "Server.h"

#include <boost/program_options.hpp>

int main(int argc, char** argv) {
	namespace po = boost::program_options;

	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help,h", "Print help")
		("inside,i", po::value<int>()->default_value(7), "Zergs inside")
		("outside,o", po::value<int>()->default_value(1), "Zergs outside")
		("seed,s", po::value<int>()->default_value(0), "RNG seed")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
	    return 1;
	}

	evil::Server server;

	server.SetZergsInside(vm["inside"].as<int>());
	server.SetZergsOutside(vm["outside"].as<int>());
	server.SetSeed(vm["seed"].as<int>());

	server.Run();
}
