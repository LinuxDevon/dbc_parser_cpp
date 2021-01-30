#include <catch2/catch.hpp>
#include "defines.hpp"
#include "dbc.hpp"

TEST_CASE("Load a simple 1 line message dbc", "[fileio]") {
	auto parser = std::unique_ptr<libdbc::DbcParser>(new libdbc::DbcParser());

	REQUIRE_THROWS_AS(parser->parse_file(DBC_FILE_2), libdbc::validity_error);

}