#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <libdbc/signal.hpp>

namespace libdbc {
	struct Message {
		Message() = delete;
		explicit Message(uint32_t id, const std::string& name, uint8_t size, const std::string& node);

        /*!
         * \brief parseSignals
         * \param data
         * \param values
         * \return
         */
        bool parseSignals(const std::vector<uint8_t>& data, std::vector<double> &values) const;
        bool parseSignals(const std::array<uint8_t,8>& data, std::vector<double>& values) const;
        bool parseSignals(const uint8_t* data, int size, std::vector<double>& values) const;

        void appendSignal(const Signal& signal);
        const std::vector<Signal> signals() const;
        uint32_t id() const;

        /*!
         * \brief prepareMessage
         * Preparing message to be able to parse signals afterwards. This speeds up parsing
         */
        void prepareMessage();

        virtual bool operator==(const Message& rhs) const;

    private:
        uint32_t m_id;
        std::string m_name;
        uint8_t m_size;
        std::string m_node;
        std::vector<Signal> m_signals; // when changing this vector m_prepared must be set to false!


        bool m_prepared{false}; // indicates if the message is prepared for parsing signals
        struct BitStruct {
            BitStruct(uint32_t size): size(size), padding(true) {}
            BitStruct(int index, uint32_t size): index(index), size(size), padding(false) {}
            BitStruct() = delete;
            int index;
            uint32_t size;
            bool padding;
        };
        std::vector<BitStruct> bitstruct;

        friend std::ostream& operator<<(std::ostream& os, const Message& dt);
	};

	std::ostream& operator<< (std::ostream &out, const Message& msg);

}

#endif // __MESSAGE_HPP__
