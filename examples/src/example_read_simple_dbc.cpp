/*
	This is the the very basic use case of the library.
	Here we are just reading a very basic dbc with one message and signal.

	All we do below is read the basic information of the dbc files
	such as the nodes, messages, and signals. This library supports
	the << operator to print out the messages and signals.

	EXAMPLE OUT:
*/
// clang-format off
/*
=== +++ ===
Version: 1.0.0
Node: DBG
Node: DRIVER
Node: IO
Node: MOTOR
Node: SENSOR
Message: {id: 500, name: IO_DEBUG, size: 4, node: IO}
Signal: {name: IO_DEBUG_test_unsigned, Multiplexed: False, Start bit: 0, Size: 8, Endianness: Little endian, Value Type: Unsigned, Min: 0.000000, Max: 0.000000, Unit: (m/s), receivers: DBG}
=== +++ ===
*/
// clang-format on

#include <libdbc/dbc.hpp>
#include <string>

// `DBC_FILE_PATH` is defined in the build system as an absolute path
static const std::string dbc_file_path = std::string(DBC_FILE_PATH) + "/HelloWorld.dbc";

int main() {
	Libdbc::DbcParser parser = Libdbc::DbcParser();
	parser.parse_file(dbc_file_path);

	std::cout << "Version: " << parser.get_version() << std::endl;

	for (auto const& node : parser.get_nodes()) {
		std::cout << "Node: " << node << std::endl;
	}

	for (auto const& message : parser.get_messages()) {
		std::cout << message << std::endl;
		for (auto const& signal : message.get_signals()) {
			std::cout << signal << std::endl;
		}
	}
}
