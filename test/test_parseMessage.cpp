#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <libdbc/dbc.hpp>

#include "common.hpp"

// Testing of parsing messages

TEST_CASE("Parse Message 1 Big Endian") {
    libdbc::DbcParser parser(true);

    const auto dbcContent = R"(BO_ 234 MSG1: 8 Vector__XXX
 SG_ Sig1 : 0|8@0- (0.1,-3) [-3276.8|-3276.7] "C" Vector__XXX
 SG_ Sig2 : 8|8@0- (0.15,7) [-3276.8|-3276.7] "C" Vector__XXX
)";

    const auto* filename = std::tmpnam(NULL);
    CHECK(create_tmp_dbc_with(filename, dbcContent));

    parser.parse_file(filename);

    SECTION("Evaluating first message") {
        std::vector<double> out_values;
        CHECK(parser.parseMessage(234, std::vector<uint8_t>({0x01, 0x02}), out_values) == true);
        CHECK(out_values.size() == 2);
        CHECK(out_values.at(0) == 0x01 * 0.1 - 3);
        CHECK(out_values.at(1) == 0x02 * 0.15 + 7);
    }

}

TEST_CASE("Parse Message 2 Big Endian") {
    libdbc::DbcParser parser(true);

    const auto dbcContent = R"(BO_ 234 MSG1: 8 Vector__XXX
 SG_ Msg1Sig1 : 0|8@0+ (1,0) [-3276.8|-3276.7] "C" Vector__XXX
 SG_ MsgSig2 : 8|8@0+ (1,0) [-3276.8|-3276.7] "C" Vector__XXX
BO_ 123 MSG2: 8 Vector__XXX
 SG_ Msg2Sig1 : 0|8@0+ (1,0) [-3276.8|-3276.7] "C" Vector__XXX
 SG_ Msg2Sig1 : 8|8@0+ (1,0) [-3276.8|-3276.7] "C" Vector__XXX
)";

    const auto* filename = std::tmpnam(NULL);
    CHECK(create_tmp_dbc_with(filename, dbcContent));

    parser.parse_file(filename);

    SECTION("Evaluating first message") {
        std::vector<double> out_values;
        CHECK(parser.parseMessage(234, std::vector<uint8_t>({0x01, 0x02}), out_values) == true);
        std::vector<double> refData{0x01, 0x02};
        CHECK(refData.size() == 2);
        CHECK(out_values.size() == refData.size());
        for (int i=0; i < refData.size(); i++) {
            CHECK(out_values.at(i) == refData.at(i));
        }
    }

    SECTION("Evaluating unknown message id") {
        std::vector<double> out_values;
        CHECK(parser.parseMessage(578, std::vector<uint8_t>({0xFF, 0xA2}), out_values) == false);
    }
}

TEST_CASE("Parse Message Big Number not aligned little endian") {
    libdbc::DbcParser parser(true);

    const auto dbcContent = R"(BO_ 337 STATUS: 8 Vector__XXX
 SG_ Value6 : 27|3@1+ (1,0) [0|7] ""  Vector__XXX
 SG_ Value5 : 16|11@1+ (0.1,-102) [-102|102] "%"  Vector__XXX
 SG_ Value2 : 8|2@1+ (1,0) [0|2] ""  Vector__XXX
 SG_ Value3 : 10|1@1+ (1,0) [0|1] ""  Vector__XXX
 SG_ Value7 : 30|2@1+ (1,0) [0|3] ""  Vector__XXX
 SG_ Value4 : 11|4@1+ (1,0) [0|3] ""  Vector__XXX
 SG_ Value1 : 0|8@1+ (1,0) [0|204] "Km/h"  Vector__XXX
)";

    const auto* filename = std::tmpnam(NULL);
    CHECK(create_tmp_dbc_with(filename, dbcContent));

    parser.parse_file(filename);

    SECTION("Evaluating first message") {
        std::vector<double> out_values;
        CHECK(parser.parseMessage(337, std::vector<uint8_t>({0, 4, 252, 19, 0, 0, 0, 0}), out_values) == true);
        std::vector<double> refData{0, 0, 1, 0, 0, 2, 0};
        CHECK(refData.size() == 7);
        CHECK(out_values.size() == refData.size());
        for (int i=0; i < refData.size(); i++) {
            CHECK(out_values.at(i) == refData.at(i));
        }
    }

    SECTION("Evaluating second message") {
        std::vector<double> out_values;
        CHECK(parser.parseMessage(337, std::vector<uint8_t>({47, 4, 60, 29, 0, 0, 0, 0}), out_values) == true);
        std::vector<double> refData{47, 0, 1, 0, 32, 3, 0};
        CHECK(refData.size() == 7);
        CHECK(out_values.size() == refData.size());
        for (int i=0; i < refData.size(); i++) {
            CHECK(out_values.at(i) == refData.at(i));
        }
    }

    SECTION("Evaluating third message") {
        std::vector<double> out_values;
        CHECK(parser.parseMessage(337, std::vector<uint8_t>({57, 4, 250, 29, 0, 0, 0, 0}), out_values) == true);
        std::vector<double> refData{57, 0, 1, 0, 51, 3, 0};
        CHECK(refData.size() == 7);
        CHECK(out_values.size() == refData.size());
        for (int i=0; i < refData.size(); i++) {
            CHECK(out_values.at(i) == refData.at(i));
        }
    }
}

// TODO: create also for big endian!
