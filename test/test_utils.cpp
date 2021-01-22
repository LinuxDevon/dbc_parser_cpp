#include <catch2/catch.hpp>
#include "defines.hpp"
#include "util/utils.hpp"

using namespace Utils;

TEST_CASE("Basic file input with safe getline", "") {
	SECTION("Verify line inputs") {
		std::ifstream TextFile;
		std::string test;

		TextFile.open(TEXT_FILE, std::ios::in);
		CHECK(TextFile.is_open());

		if(TextFile.is_open()) {
			SafeString::getline(TextFile, test);
			REQUIRE(test == "This is a non dbc formatted file.");
			SafeString::getline(TextFile, test);
			REQUIRE(test == "");
			SafeString::getline(TextFile, test);
			REQUIRE(test == "Make sure things pass with this");
			SafeString::getline(TextFile, test);
			REQUIRE(test == "Who knows 	what might happen.");

			TextFile.close();
		}
	}
}
