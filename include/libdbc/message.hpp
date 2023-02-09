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

        enum class ParseSignalsStatus {
            Success,
            ErrorMessageToLong,
            ErrorBigEndian,
            ErrorUnknownID,
            ErrorInvalidConversion,
        };

        /*!
         * \brief parseSignals
         * \param data
         * \param values
         * \return
         */
        ParseSignalsStatus parseSignals(const std::vector<uint8_t>& data, std::vector<double> &values) const;

        void appendSignal(const Signal& signal);
        const std::vector<Signal> signals() const;
        uint32_t id() const;

        virtual bool operator==(const Message& rhs) const;

    private:
        uint32_t m_id;
        std::string m_name;
        uint8_t m_size;
        std::string m_node;
        std::vector<Signal> m_signals;

        friend std::ostream& operator<<(std::ostream& os, const Message& dt);
	};

	std::ostream& operator<< (std::ostream &out, const Message& msg);

}

#endif // __MESSAGE_HPP__
