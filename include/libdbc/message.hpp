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

        bool parseSignals(const std::vector<uint8_t> data, std::vector<double> &values) const;

        /*!
         * \brief prepareMessage
         * Preparing message to be able to parse signals afterwards. This speeds up parsing
         */
        void prepareMessage();

        virtual bool operator==(const Message& rhs) const;

    private:
        bool prepared{false}; // indicates if the message is prepared for parsing signals
        struct BitStruct {
            BitStruct(const std::string& name, uint32_t size, bool padding): name(name), size(size), padding(padding) {}
            BitStruct() = delete;
            std::string name;
            uint32_t size;
            bool padding;
        };
        std::vector<BitStruct> bitstruct;
	};

	std::ostream& operator<< (std::ostream &out, const Message& msg);

}

#endif // __MESSAGE_HPP__
