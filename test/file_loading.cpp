// #include "catch.hpp"
#include <catch2/catch.hpp>
#include "defines.hpp"
#include "dbc.hpp"

TEST_CASE("Basic file input with safe getline", "") {

	SECTION("Verify line inputs") {
		std::ifstream TextFile;
		std::string test;

		TextFile.open(TEXT_FILE, std::ios::in);
		CHECK(TextFile.is_open());

		if(TextFile.is_open()) {
			SafeStr::getline(TextFile, test);
			REQUIRE(test == "This is a non dbc formatted file.");
			SafeStr::getline(TextFile, test);
			REQUIRE(test == "");
			SafeStr::getline(TextFile, test);
			REQUIRE(test == "Make sure things pass with this");
			SafeStr::getline(TextFile, test);
			REQUIRE(test == "Who knows 	what might happen.");

			TextFile.close();
		}
	}
}