#include "exceptions/error.hpp"
#include "util/utils.hpp"

#include <regex>

namespace libdbc {

	class Parser {
	public:
		virtual ~Parser() = default;

		virtual void parse_file(const std::string& file) = 0;

	protected:


	};

	class DbcParser : public Parser {
	public:
		DbcParser() : version(""), version_re("(VERSION)\\s\"(.*)\"") {}

		virtual ~DbcParser() = default;

		virtual void parse_file(const std::string& file) final override {
			std::ifstream s(file.c_str());
			std::string line;

			parse_dbc_header(s);

			while(!s.eof()) {
				Utils::SafeString::get_line(s, line);

			}

		}

		std::string get_version() const {
			return version;
		}

	private:
		std::string version;

		const std::regex version_re;

		void parse_dbc_header(std::istream& file_stream) {
			std::string line;
			std::smatch match;

			Utils::SafeString::get_line(file_stream, line);

			if(!std::regex_search(line, match, version_re)) {
				throw validity_error();
			}

			version = match.str(2);

		}

	};

	struct Message {

	};

}
