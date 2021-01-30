#include "exceptions/error.hpp"

namespace libdbc {

	class Parser {
	public:
		virtual ~Parser() = default;

		virtual void parse_file(const std::string& file) = 0;
	};

	class DbcParser : public Parser {
	public:
		virtual ~DbcParser() = default;

		virtual void parse_file(const std::string& file) final override {
			throw validity_error();
		}

	};

}
