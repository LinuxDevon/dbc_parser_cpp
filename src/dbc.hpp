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
		DbcParser() : version(""), version_re("^(VERSION)\\s\"(.*)\""),
					  bit_timing_re("^(BS_:)"), name_space_re("^(NS_)\\s\\:") {

		}

		virtual ~DbcParser() = default;

		virtual void parse_file(const std::string& file) final override {
			std::ifstream s(file.c_str());
			std::string line;

			parse_dbc_header(s);

			while(!s.eof()) {
				utils::StreamHandler::get_line(s, line);

			}

		}

		std::string get_version() const {
			return version;
		}

	private:
		std::string version;

		const std::regex version_re;
		const std::regex bit_timing_re;
		const std::regex name_space_re;

		void parse_dbc_header(std::istream& file_stream) {
			std::string line;
			std::smatch match;
			bool is_blank = true;
			bool not_blank = true;

			utils::StreamHandler::get_line(file_stream, line);

			if(!std::regex_search(line, match, version_re)) {
				throw validity_error();
			}

			version = match.str(2);

			utils::StreamHandler::get_next_non_blank_line( file_stream, line );

			utils::StreamHandler::skip_to_next_blank_line( file_stream, line );

			utils::StreamHandler::get_next_non_blank_line( file_stream, line );

			if(!std::regex_search(line, match, bit_timing_re))
				throw validity_error();

		}
		std::string trim(const std::string& line)
		{
			const char* WhiteSpace = " \t\v\r\n";
			std::size_t start = line.find_first_not_of(WhiteSpace);
			std::size_t end = line.find_last_not_of(WhiteSpace);
			return start == end ? std::string() : line.substr(start, end - start + 1);
		}

	};


	struct Message {

	};

}
