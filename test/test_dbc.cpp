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

/*!
 * \brief TEST_CASE
 * Test negative values in offset, min, max
 */
TEST_CASE("Testing negative values") {
  const auto* filename = std::tmpnam(NULL);

  auto* file = std::fopen(filename, "w");
  CHECK(file);

  std::fputs(PRIMITIVE_DBC.c_str(), file);
  std::fputs(R"(BO_ 234 MSG1: 8 Vector__XXX
 SG_ Sig1 : 55|16@0- (0.1,0) [-3276.8|-3276.7] "C" Vector__XXX
 SG_ Sig2 : 39|16@0- (0.1,0) [-3276.8|-3276.7] "C" Vector__XXX
 SG_ Sig3 : 23|16@0- (10,0) [-3276.8|-3276.7] "C" Vector__XXX
 SG_ Sig4 : 7|16@0- (1,-10) [0|32767] "" Vector__XXX)", file);
  std::fclose(file);

  auto parser = libdbc::DbcParser();
  parser.parse_file(std::string(filename));

  REQUIRE(parser.get_messages().size() == 1);
  REQUIRE(parser.get_messages().at(0).signals.size() == 4);

  SECTION("Evaluating first message") {
    const auto signal = parser.get_messages().at(0).signals.at(0);
    REQUIRE(signal.factor == 0.1);
    REQUIRE(signal.offset == 0);
    REQUIRE(signal.min == -3276.8);
    REQUIRE(signal.max == -3276.7);
  }
  SECTION("Evaluating second message") {
    const auto signal = parser.get_messages().at(0).signals.at(1);
    REQUIRE(signal.factor == 0.1);
    REQUIRE(signal.offset == 0);
    REQUIRE(signal.min == -3276.8);
    REQUIRE(signal.max == -3276.7);
  }
  SECTION("Evaluating third message"){
    const auto signal = parser.get_messages().at(0).signals.at(2);
    REQUIRE(signal.factor == 10);
    REQUIRE(signal.offset == 0);
    REQUIRE(signal.min == -3276.8);
    REQUIRE(signal.max == -3276.7);
  }
  SECTION("Evaluating fourth message"){
    const auto signal = parser.get_messages().at(0).signals.at(3);
    REQUIRE(signal.factor == 1);
    REQUIRE(signal.offset == -10);
    REQUIRE(signal.min == 0);
    REQUIRE(signal.max == 32767);
  }
}


TEST_CASE("Special characters in unit") {
    const auto* filename = std::tmpnam(NULL);

    auto* file = std::fopen(filename, "w");
    CHECK(file);

    std::fputs(PRIMITIVE_DBC.c_str(), file);
    std::fputs(R"(BO_ 234 MSG1: 8 Vector__XXX
 SG_ Speed : 0|8@1+ (1,0) [0|204] "Km/h"  DEVICE1,DEVICE2,DEVICE3)", file);
    std::fclose(file);

    auto parser = libdbc::DbcParser();
    parser.parse_file(std::string(filename));

    REQUIRE(parser.get_messages().size() == 1);
    REQUIRE(parser.get_messages().at(0).signals.size() == 1);
    {
      const auto signal = parser.get_messages().at(0).signals.at(0);
      REQUIRE(signal.unit.compare("Km/h") == 0);
    }
}
