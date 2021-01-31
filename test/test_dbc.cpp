#include <catch2/catch.hpp>
#include "defines.hpp"
#include "dbc.hpp"

TEST_CASE("Testing dbc file loading", "[fileio]") {
	auto parser = std::unique_ptr<libdbc::DbcParser>(new libdbc::DbcParser());

	SECTION("Loading a non dbc file should throw an error", "[error]") {
		REQUIRE_THROWS_AS(parser->parse_file(TEXT_FILE), libdbc::validity_error);
	}

	SECTION("Loading a single simple dbc file", "[dbc]") {
		std::vector<libdbc::Message> messages;

		parser->parse_file(DBC_FILE_2);

		REQUIRE(parser->get_version() == "1.0.0");
	}

}