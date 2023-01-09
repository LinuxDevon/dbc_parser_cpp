#include <catch2/catch_test_macros.hpp>
#include "defines.hpp"
#include <libdbc/dbc.hpp>

TEST_CASE("Testing dbc file loading error issues", "[fileio][error]") {
	auto parser = std::unique_ptr<libdbc::DbcParser>(new libdbc::DbcParser());

	SECTION("Loading a non dbc file should throw an error", "[error]") {
		REQUIRE_THROWS_AS(parser->parse_file(TEXT_FILE), libdbc::validity_error);
	}

	SECTION("Loading a dbc with bad headers throws an error", "[error]") {
		REQUIRE_THROWS_AS(parser->parse_file(MISSING_VERSION_DBC_FILE), libdbc::validity_error);
	}

	SECTION("Loading a dbc without the required bit timing section (BS_:)", "[error]") {
		REQUIRE_THROWS_AS(parser->parse_file(MISSING_BIT_TIMING_DBC_FILE), libdbc::validity_error);
	}

	SECTION("Loading a dbc with some missing namespace section tags (NS_ :)", "[error]") {
		// Confusion about this type of error. it appears that the header isn't
		// very well standardized for now we ignore this type of error.
		CHECK_NOTHROW(parser->parse_file(MISSING_NEW_SYMBOLS_DBC_FILE));
	}

	SECTION("Verify that what() method is accessible for all exceptions", "[error]")
	{
		auto generic_error = libdbc::exception();
		REQUIRE(generic_error.what() == "libdbc exception occurred");

		auto validity_check = libdbc::validity_error();
		REQUIRE(validity_check.what() == "Invalid DBC file");
	}
}

TEST_CASE("Testing dbc file loading", "[fileio]") {
	auto parser = std::unique_ptr<libdbc::DbcParser>(new libdbc::DbcParser());

	SECTION("Loading a single simple dbc file", "[dbc]") {
		std::vector<std::string> nodes = {"DBG", "DRIVER", "IO", "MOTOR", "SENSOR"};

		libdbc::Message msg(500, "IO_DEBUG", 4, "IO");

		std::vector<std::string> receivers{"DBG"};
		libdbc::Signal sig("IO_DEBUG_test_unsigned", false, 0, 8, true, false, 1, 0, 0, 0, "", receivers);
 		msg.signals.push_back(sig);

		std::vector<libdbc::Message> msgs = {msg};

		parser->parse_file(SIMPLE_DBC_FILE);

		REQUIRE(parser->get_version() == "1.0.0");

		REQUIRE(parser->get_nodes() == nodes);

		REQUIRE(parser->get_messages() == msgs);

		REQUIRE(parser->get_messages().front().signals == msg.signals);
	}

}