#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include <string>
#include <vector>
#include <iostream>
#include <libdbc/signal.hpp>

namespace libdbc {
	struct Message {
		uint32_t id;
		std::string name;
		uint8_t size;
		std::string node;
		std::vector<Signal> signals;

		Message() = delete;
		explicit Message(uint32_t id, const std::string& name, uint8_t size, const std::string& node);

		virtual bool operator==(const Message& rhs) const;
	};

	std::ostream& operator<< (std::ostream &out, const Message& msg);

}

#endif // __MESSAGE_HPP__
