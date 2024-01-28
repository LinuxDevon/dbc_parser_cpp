#include "common.hpp"
#include "defines.hpp"
#include <libdbc/libdbc.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

TEST_CASE("Testing dbc file loading", "[fileio]") {
	auto parser = std::unique_ptr<libdbc::DbcParser>(new libdbc::DbcParser());

	SECTION("Loading a single simple dbc file", "[dbc]") {
		std::vector<std::string> nodes = {"DBG", "DRIVER", "IO", "MOTOR", "SENSOR"};

		libdbc::Message msg(500, "IO_DEBUG", 4, "IO");

		std::vector<std::string> receivers{"DBG"};
		libdbc::Signal sig("IO_DEBUG_test_unsigned", false, 0, 8, false, false, 1, 0, 0, 0, "", receivers);
		msg.appendSignal(sig);

		std::vector<libdbc::Message> msgs = {msg};

		parser->parse_file(SIMPLE_DBC_FILE);

		REQUIRE(parser->get_version() == "1.0.0");

		REQUIRE(parser->get_nodes() == nodes);

		REQUIRE(parser->get_messages() == msgs);

		REQUIRE(parser->get_messages().front().getSignals() == msg.getSignals());
	}
}

TEST_CASE("Testing  big endian, little endian") {
	std::string dbc_contents = PRIMITIVE_DBC + R"(BO_ 234 MSG1: 8 Vector__XXX
 SG_ Sig1 : 55|16@0- (0.1,0) [-3276.8|-3276.7] "C" Vector__XXX
 SG_ Sig2 : 39|16@1- (0.1,0) [-3276.8|-3276.7] "C" Vector__XXX)";
	const auto filename = create_temporary_dbc_with(dbc_contents.c_str());

	auto parser = libdbc::DbcParser();
	parser.parse_file(filename.c_str());

	REQUIRE(parser.get_messages().size() == 1);
	REQUIRE(parser.get_messages().at(0).name() == "MSG1");
	REQUIRE(parser.get_messages().at(0).size() == 8);
	REQUIRE(parser.get_messages().at(0).getSignals().size() == 2);
	{
		const auto signal = parser.get_messages().at(0).getSignals().at(0);
		REQUIRE(signal.is_bigendian == true);
	}
	{
		const auto signal = parser.get_messages().at(0).getSignals().at(1);
		REQUIRE(signal.is_bigendian == false);
	}
}
