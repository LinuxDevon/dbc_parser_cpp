#include "common.hpp"
#include "defines.hpp"
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
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

	SECTION("Verify that what() method is accessible for all exceptions", "[error]") {
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

		REQUIRE(parser->get_messages().front().getSignals() == msg.getSignals());
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
 SG_ Sig2 : 39|16@1- (0.1,0) [-3276.8|-3276.7] "C" Vector__XXX)",
			   file);
	std::fclose(file);

	auto parser = libdbc::DbcParser();
	parser.parse_file(filename);

	REQUIRE(parser.get_messages().size() == 1);
	REQUIRE(parser.get_messages().at(0).name() == "MSG1");
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
	REQUIRE(parser.get_messages().at(0).name() == "MSG1");
	REQUIRE(parser.get_messages().at(0).getSignals().size() == 4);

	SECTION("Evaluating first message") {
		const auto signal = parser.get_messages().at(0).getSignals().at(0);
		REQUIRE(signal.factor == 0.1);
		REQUIRE(signal.offset == 0);
		REQUIRE(signal.min == -3276.8);
		REQUIRE(signal.max == -3276.7);
	}
	SECTION("Evaluating second message") {
		const auto signal = parser.get_messages().at(0).getSignals().at(1);
		REQUIRE(signal.factor == 0.1);
		REQUIRE(signal.offset == 0);
		REQUIRE(signal.min == -3276.8);
		REQUIRE(signal.max == -3276.7);
	}
	SECTION("Evaluating third message") {
		const auto signal = parser.get_messages().at(0).getSignals().at(2);
		REQUIRE(signal.factor == 10);
		REQUIRE(signal.offset == 0);
		REQUIRE(signal.min == -3276.8);
		REQUIRE(signal.max == -3276.7);
	}
	SECTION("Evaluating fourth message") {
		const auto signal = parser.get_messages().at(0).getSignals().at(3);
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
	REQUIRE(parser.get_messages().at(0).name() == "MSG1");
	REQUIRE(parser.get_messages().at(0).getSignals().size() == 1);
	SECTION("Checking that signal with special characters as unit is parsed correctly") {
		const auto signal = parser.get_messages().at(0).getSignals().at(0);
		REQUIRE(signal.unit.compare("Km/h") == 0);
	}
}

TEST_CASE("Signal Value Description") {
	const auto* filename = std::tmpnam(NULL);

	create_tmp_dbc_with(filename, R"(BO_ 234 MSG1: 8 Vector__XXX
 SG_ State1 : 0|8@1+ (1,0) [0|200] "Km/h"  DEVICE1,DEVICE2,DEVICE3
 SG_ State2 : 0|8@1+ (1,0) [0|204] ""  DEVICE1,DEVICE2,DEVICE3
VAL_ 234 State1 123 "Description 1" 0 "Description 2" 90903489 "Big value and special characters &$Â§())!" ;)");

	auto parser = libdbc::DbcParser();
	parser.parse_file(filename);

	REQUIRE(parser.get_messages().size() == 1);
	REQUIRE(parser.get_messages().at(0).name() == "MSG1");
	REQUIRE(parser.get_messages().at(0).getSignals().size() == 2);

	REQUIRE(parser.get_messages().at(0).getSignals().at(0).svDescriptions.size() == 3);
	REQUIRE(parser.get_messages().at(0).getSignals().at(1).svDescriptions.size() == 0);

	const auto signal = parser.get_messages().at(0).getSignals().at(0);
	REQUIRE(signal.svDescriptions.at(0).value == 123);
	REQUIRE(signal.svDescriptions.at(0).description == "Description 1");
	REQUIRE(signal.svDescriptions.at(1).value == 0);
	REQUIRE(signal.svDescriptions.at(1).description == "Description 2");
	REQUIRE(signal.svDescriptions.at(2).value == 90903489);
	REQUIRE(signal.svDescriptions.at(2).description == "Big value and special characters &$Â§())!");
}

TEST_CASE("Signal Value Description Extended CAN id") {
	/*
	 * It should not crash, even extended CAN id is used
	 */
	const auto* filename = std::tmpnam(NULL);

	create_tmp_dbc_with(filename, R"(BO_ 3221225472 MSG1: 8 Vector__XXX
 SG_ State1 : 0|8@1+ (1,0) [0|200] "Km/h"  DEVICE1,DEVICE2,DEVICE3
 SG_ State2 : 0|8@1+ (1,0) [0|204] ""  DEVICE1,DEVICE2,DEVICE3
VAL_ 3221225472 State1 123 "Description 1" 0 "Description 2" 4000000000 "Big value and special characters &$Â§())!" ;)");

	auto parser = libdbc::DbcParser();
	parser.parse_file(filename);

	REQUIRE(parser.get_messages().size() == 1);
	REQUIRE(parser.get_messages().at(0).name() == "MSG1");
	REQUIRE(parser.get_messages().at(0).getSignals().size() == 2);

	REQUIRE(parser.get_messages().at(0).getSignals().at(0).svDescriptions.size() == 3);
	REQUIRE(parser.get_messages().at(0).getSignals().at(1).svDescriptions.size() == 0);

	const auto signal = parser.get_messages().at(0).getSignals().at(0);
	REQUIRE(signal.svDescriptions.at(0).value == 123);
	REQUIRE(signal.svDescriptions.at(0).description == "Description 1");
	REQUIRE(signal.svDescriptions.at(1).value == 0);
	REQUIRE(signal.svDescriptions.at(1).description == "Description 2");
	REQUIRE(signal.svDescriptions.at(2).value == 4000000000);
	REQUIRE(signal.svDescriptions.at(2).description == "Big value and special characters &$Â§())!");
}

TEST_CASE("Signal Value Multiple VAL_") {
	/*
	 * It should not crash, even extended CAN id is used
	 */
	const auto* filename = std::tmpnam(NULL);

	create_tmp_dbc_with(filename, R"(BO_ 3221225472 MSG1: 8 Vector__XXX
 SG_ State1 : 0|8@1+ (1,0) [0|200] "Km/h"  DEVICE1,DEVICE2,DEVICE3
 SG_ State2 : 0|8@1+ (1,0) [0|204] ""  DEVICE1,DEVICE2,DEVICE3"
BO_ 123 MSG2: 8 Vector__XXX
 SG_ State1 : 0|8@1+ (1,0) [0|200] "Km/h"  DEVICE1,DEVICE2,DEVICE3
 SG_ State2 : 0|8@1+ (1,0) [0|204] ""  DEVICE1,DEVICE2,DEVICE3
VAL_ 3221225472 State1 123 "Description 1" 0 "Description 2" ;
VAL_ 123 State1 123 "Description 3" 0 "Description 4" ;)");

	auto parser = libdbc::DbcParser();
	parser.parse_file(filename);

	REQUIRE(parser.get_messages().size() == 2);
	REQUIRE(parser.get_messages().at(0).name() == "MSG1");
	REQUIRE(parser.get_messages().at(1).name() == "MSG2");

	REQUIRE(parser.get_messages().at(0).getSignals().size() == 2);

	REQUIRE(parser.get_messages().at(0).getSignals().at(0).svDescriptions.size() == 2);
	REQUIRE(parser.get_messages().at(0).getSignals().at(1).svDescriptions.size() == 0);
	REQUIRE(parser.get_messages().at(1).getSignals().at(0).svDescriptions.size() == 2);
	REQUIRE(parser.get_messages().at(1).getSignals().at(1).svDescriptions.size() == 0);

	const auto signal = parser.get_messages().at(0).getSignals().at(0);
	REQUIRE(signal.svDescriptions.at(0).value == 123);
	REQUIRE(signal.svDescriptions.at(0).description == "Description 1");
	REQUIRE(signal.svDescriptions.at(1).value == 0);
	REQUIRE(signal.svDescriptions.at(1).description == "Description 2");

	const auto signal2 = parser.get_messages().at(1).getSignals().at(0);
	REQUIRE(signal2.svDescriptions.at(0).value == 123);
	REQUIRE(signal2.svDescriptions.at(0).description == "Description 3");
	REQUIRE(signal2.svDescriptions.at(1).value == 0);
	REQUIRE(signal2.svDescriptions.at(1).description == "Description 4");
}
