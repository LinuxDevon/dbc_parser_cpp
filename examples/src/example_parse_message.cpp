/*

*/

#include <libdbc/dbc.hpp>
#include <string>

// `DBC_FILE_PATH` is defined in the build system as an absolute path
static const std::string dbc_file_path = std::string(DBC_FILE_PATH) + "/HelloWorld.dbc";

int main() {
	Libdbc::DbcParser parser = Libdbc::DbcParser();
	parser.parse_file(dbc_file_path);

	// Not required. Just using this to show the message information in the output.
	std::cout << parser.get_messages()[0] << std::endl;

	Libdbc::Message::ParseSignalsStatus status = parser.parse_message(337, std::vector<uint8_t>({0, 4, 252, 19, 0, 0, 0, 0}), out_values);
	if (status != Libdbc::Message::ParseSignalsStatus::Success) {
		return 1;
	}
}
