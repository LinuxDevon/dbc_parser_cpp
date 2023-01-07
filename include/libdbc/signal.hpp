
#ifndef __SIGNAL_HPP__
#define __SIGNAL_HPP__

#include <string>
#include <vector>
#include <iostream>

namespace libdbc {
	struct Signal {
		std::string name;
		bool is_multiplexed;
		uint32_t start_bit;
		uint32_t size;
		bool is_bigendian;
		bool is_signed;
		double factor;
		double offset;
		double min;
		double max;
		std::string unit;
		std::vector<std::string> receivers;

		Signal() = delete;
		explicit Signal(std::string name, bool is_multiplexed, uint32_t start_bit, uint32_t size, bool is_bigendian, bool is_signed, double factor, double offset, double min, double max, std::string unit, std::vector<std::string> recievers);

		virtual bool operator==(const Signal& rhs) const;
        bool operator< (const Signal& rhs) const;
	};

	std::ostream& operator<< (std::ostream &out, const Signal& sig);

}

#endif // __SIGNAL_HPP__
