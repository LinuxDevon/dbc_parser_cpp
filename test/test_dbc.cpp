#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include "defines.hpp"
#include <libdbc/dbc.hpp>
#include "common.hpp"


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
		libdbc::Signal sig("IO_DEBUG_test_unsigned", false, 0, 8, false, false, 1, 0, 0, 0, "", receivers);
        msg.appendSignal(sig);

		std::vector<libdbc::Message> msgs = {msg};

		parser->parse_file(SIMPLE_DBC_FILE);

		REQUIRE(parser->get_version() == "1.0.0");

		REQUIRE(parser->get_nodes() == nodes);

		REQUIRE(parser->get_messages() == msgs);

        REQUIRE(parser->get_messages().front().signals() == msg.signals());
	}

}

TEST_CASE("Testing  big endian, little endian") {
  const auto* filename = std::tmpnam(NULL);

  auto* file = std::fopen(filename, "w");
  CHECK(file);

  std::fputs(PRIMITIVE_DBC.c_str(), file);
  // first big endian
  // second little endian
  std::fputs(R"(BO_ 234 MSG1: 8 Vector__XXX
 SG_ Sig1 : 55|16@0- (0.1,0) [-3276.8|-3276.7] "C" Vector__XXX
 SG_ Sig2 : 39|16@1- (0.1,0) [-3276.8|-3276.7] "C" Vector__XXX)", file);
  std::fclose(file);

  auto parser = libdbc::DbcParser();
  parser.parse_file(filename);

  REQUIRE(parser.get_messages().size() == 1);
  REQUIRE(parser.get_messages().at(0).signals().size() == 2);
  {
    const auto signal = parser.get_messages().at(0).signals().at(0);
    REQUIRE(signal.is_bigendian == true);
  }
  {
    const auto signal = parser.get_messages().at(0).signals().at(1);
    REQUIRE(signal.is_bigendian == false);
  }
}

TEST_CASE("Testing negative values") {
  const auto* filename = std::tmpnam(NULL);

  create_tmp_dbc_with(filename, R"(BO_ 234 MSG1: 8 Vector__XXX
 SG_ Sig1 : 55|16@0- (0.1,0) [-3276.8|-3276.7] "C" Vector__XXX
 SG_ Sig2 : 39|16@0- (0.1,0) [-3276.8|-3276.7] "C" Vector__XXX
 SG_ Sig3 : 23|16@0- (10,0) [-3276.8|-3276.7] "C" Vector__XXX
 SG_ Sig4 : 7|16@0- (1,-10) [0|32767] "" Vector__XXX)");

  auto parser = libdbc::DbcParser();
  parser.parse_file(filename);

  REQUIRE(parser.get_messages().size() == 1);
  REQUIRE(parser.get_messages().at(0).signals().size() == 4);

  SECTION("Evaluating first message") {
    const auto signal = parser.get_messages().at(0).signals().at(0);
    REQUIRE(signal.factor == 0.1);
    REQUIRE(signal.offset == 0);
    REQUIRE(signal.min == -3276.8);
    REQUIRE(signal.max == -3276.7);
  }
  SECTION("Evaluating second message") {
    const auto signal = parser.get_messages().at(0).signals().at(1);
    REQUIRE(signal.factor == 0.1);
    REQUIRE(signal.offset == 0);
    REQUIRE(signal.min == -3276.8);
    REQUIRE(signal.max == -3276.7);
  }
  SECTION("Evaluating third message"){
    const auto signal = parser.get_messages().at(0).signals().at(2);
    REQUIRE(signal.factor == 10);
    REQUIRE(signal.offset == 0);
    REQUIRE(signal.min == -3276.8);
    REQUIRE(signal.max == -3276.7);
  }
  SECTION("Evaluating fourth message"){
    const auto signal = parser.get_messages().at(0).signals().at(3);
    REQUIRE(signal.factor == 1);
    REQUIRE(signal.offset == -10);
    REQUIRE(signal.min == 0);
    REQUIRE(signal.max == 32767);
  }
}

TEST_CASE("Special characters in unit") {
    const auto* filename = std::tmpnam(NULL);

    create_tmp_dbc_with(filename, R"(BO_ 234 MSG1: 8 Vector__XXX
 SG_ Speed : 0|8@1+ (1,0) [0|204] "Km/h"  DEVICE1,DEVICE2,DEVICE3)");


    auto parser = libdbc::DbcParser();
    parser.parse_file(filename);

    REQUIRE(parser.get_messages().size() == 1);
    REQUIRE(parser.get_messages().at(0).signals().size() == 1);
    SECTION("Checking that signal with special characters as unit is parsed correctly") {
      const auto signal = parser.get_messages().at(0).signals().at(0);
      REQUIRE(signal.unit.compare("Km/h") == 0);
    }
}

TEST_CASE("Parse Message little endian") {
    const auto* filename = std::tmpnam(NULL);

    auto* file = std::fopen(filename, "w");
    CHECK(file);

    std::fputs(PRIMITIVE_DBC.c_str(), file);
    std::fputs(R"(BO_ 541 STATUS: 8 DEVICE1
 SG_ Temperature : 48|16@1+ (0.01,-40) [-40|125] "C"  DEVICE1
 SG_ SOH : 0|16@1+ (0.01,0) [0|100] "%"  DEVICE1
 SG_ SOE : 32|16@1+ (0.01,0) [0|100] "%"  DEVICE1
 SG_ SOC : 16|16@1+ (0.01,0) [0|100] "%"  DEVICE1)", file);
    std::fclose(file);

    libdbc::DbcParser p(true);
    p.parse_file(filename);

    std::vector<uint8_t> data{0x08, 0x27, 0xa3, 0x22, 0xe5, 0x1f, 0x45, 0x14}; // little endian
    std::vector<double> result_values;
    REQUIRE(p.parseMessage(0x21d, data, result_values) == true);
    REQUIRE(result_values.size() == 4);
    REQUIRE(Catch::Approx(result_values.at(0)) == 99.92);
    REQUIRE(Catch::Approx(result_values.at(1)) == 88.67);
    REQUIRE(Catch::Approx(result_values.at(2)) == 81.65);
    REQUIRE(Catch::Approx(result_values.at(3)) == 11.89);
}

TEST_CASE("Parse Message big endian") {
    const auto* filename = std::tmpnam(NULL);

    auto* file = std::fopen(filename, "w");
    CHECK(file);

    std::fputs(PRIMITIVE_DBC.c_str(), file);
    std::fputs(R"(BO_ 541 STATUS: 8 DEVICE1
 SG_ Temperature : 48|16@0+ (0.01,-40) [-40|125] "C"  DEVICE1
 SG_ SOH : 0|16@0+ (0.01,0) [0|100] "%"  DEVICE1
 SG_ SOE : 32|16@0+ (0.01,0) [0|100] "%"  DEVICE1
 SG_ SOC : 16|16@0+ (0.01,0) [0|100] "%"  DEVICE1)", file);
    std::fclose(file);

    libdbc::DbcParser p(true);
    p.parse_file(filename);

    std::vector<uint8_t> data{0x27, 0x08, 0x22, 0xa3, 0x1f, 0xe5, 0x14, 0x45}; // big endian
    std::vector<double> result_values;
    REQUIRE(p.parseMessage(0x21d, data, result_values) == true);
    REQUIRE(result_values.size() == 4);
    REQUIRE(Catch::Approx(result_values.at(0)) == 99.92);
    REQUIRE(Catch::Approx(result_values.at(1)) == 88.67);
    REQUIRE(Catch::Approx(result_values.at(2)) == 81.65);
    REQUIRE(Catch::Approx(result_values.at(3)) == 11.89);
}
