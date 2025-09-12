#include <cstddef>
#include <fast_float/fast_float.h>
#include <istream>
#include <libdbc/utils/utils.hpp>
#include <regex>
#include <string>

namespace Utils {

std::istream& StreamHandler::get_line(std::istream& stream, std::string& line) {
	std::getline(stream, line);
    if (!line.empty() && line.back() == '\r') {
        line.pop_back(); // strip CR from CRLF
    }
    return stream;
}

std::istream& StreamHandler::get_next_non_blank_line(std::istream& stream, std::string& line) {
	for (;;) {
        Utils::StreamHandler::get_line(stream, line);  // must strip trailing '\r'

        if (!stream) {                    // EOF or error after attempt to read
            return stream;
        }

        if (!Utils::isWhitespaceOrEmpty(line)) {
            return stream;                // found a non-blank line
        }
    }
}

std::istream& StreamHandler::skip_to_next_blank_line(std::istream& stream, std::string& line) {
	for (;;) {
        Utils::StreamHandler::get_line(stream, line);  // must strip trailing '\r'

        if (!stream) {                    // EOF or error after attempt to read
            line.clear();                 // test expects "" at/after EOF; remove me if the behaviour is to be fixed.
            return stream;
        }

        if (Utils::isWhitespaceOrEmpty(line)) {
            return stream;                // found a blank line
        }
    }
}

double String::convert_to_double(const std::string& value, double default_value) {
	double converted_value = default_value;
	// NOLINTNEXTLINE -- Trying to iterators on the value causes the test to infinitly hang on windows builds
	fast_float::from_chars(value.data(), value.data() + value.size(), converted_value);
	return converted_value;
}

} // Namespace Utils
