#include <catch2/catch.hpp>
#include "defines.hpp"
#include "util/utils.hpp"

using namespace utils;

TEST_CASE("Basic file input with safe get_line that is non line ending specific", "") {
	SECTION("Verify various line ending input files") {
		std::ifstream TextFile;
		std::string test;

		TextFile.open(TEXT_FILE, std::ios::in);
		CHECK(TextFile.is_open());

		if(TextFile.is_open()) {
			SafeString::get_line(TextFile, test);
			REQUIRE(test == "This is a non dbc formatted file.");
			SafeString::get_line(TextFile, test);
			REQUIRE(test == "");
			SafeString::get_line(TextFile, test);
			REQUIRE(test == "Make sure things pass with this");
			SafeString::get_line(TextFile, test);
			REQUIRE(test == "Who knows 	what might happen.");

			TextFile.close();
		}
	}
}
