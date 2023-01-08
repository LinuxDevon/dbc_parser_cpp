#include <algorithm>
#include <libdbc/message.hpp>

#include <bitstream.h>

namespace libdbc {
	Message::Message(uint32_t id, const std::string& name, uint8_t size, const std::string& node) :
        m_id(id), m_name(name), m_size(size), m_node(node) {}

	bool Message::operator==(const Message& rhs) const {
        return (m_id == rhs.id()) && (m_name == rhs.m_name) &&
               (m_size == rhs.m_size) && (m_node == rhs.m_node);
    }

    bool Message::parseSignals(const std::vector<uint8_t> data, std::vector<double>& values) const {
        if (!m_prepared)
            return false;

        bitstream_reader_t reader;
        bitstream_reader_init(&reader, data.data());

        double v;
        for (const auto& bs: bitstruct) {
            if (bs.padding) {
                for (uint32_t i=0; i < bs.size; i++)
                    bitstream_reader_read_bit(&reader);
            } else {
                const auto& signal = m_signals.at(bs.index);
                if (signal.is_bigendian) {

                    switch (bs.size) {
                        case 1: v = static_cast<double>(bitstream_reader_read_bit(&reader)); break;
                    case 8: v = static_cast<double>(bitstream_reader_read_u8(&reader)); break;
                    case 16: {
                        uint16_t tmp = bitstream_reader_read_u8(&reader);
                        tmp |= (uint16_t)bitstream_reader_read_u8(&reader) << 8;
                        if (signal.is_signed)
                            v = static_cast<double>(static_cast<int16_t>(tmp));
                        else
                            v = static_cast<double>(tmp);
                        break;
                    }
                    case 32: {
                        uint32_t tmp = bitstream_reader_read_u8(&reader);
                        tmp |= (uint32_t)bitstream_reader_read_u8(&reader) << 8;
                        tmp |= (uint32_t)bitstream_reader_read_u8(&reader) << 16;
                        tmp |= (uint32_t)bitstream_reader_read_u8(&reader) << 24;
                        if (signal.is_signed)
                            v = static_cast<double>(static_cast<int32_t>(tmp));
                        else
                            v = static_cast<double>(tmp);
                        break;
                    }
                    case 64: {
                        uint64_t tmp = bitstream_reader_read_u8(&reader);
                        tmp |= (uint64_t)bitstream_reader_read_u8(&reader) << 8;
                        tmp |= (uint64_t)bitstream_reader_read_u8(&reader) << 16;
                        tmp |= (uint64_t)bitstream_reader_read_u8(&reader) << 24;
                        tmp |= (uint64_t)bitstream_reader_read_u8(&reader) << 32;
                        tmp |= (uint64_t)bitstream_reader_read_u8(&reader) << 40;
                        tmp |= (uint64_t)bitstream_reader_read_u8(&reader) << 48;
                        tmp |= (uint64_t)bitstream_reader_read_u8(&reader) << 56;
                        if (signal.is_signed)
                            v = static_cast<double>(static_cast<int64_t>(tmp));
                        else
                            v = static_cast<double>(tmp);
                        break;
                    }
                    default: {
                        // TODO: possible to implement bigendian and sign?
                        uint64_t value = 0;
                        for (uint32_t i=0; i < bs.size; i++) {
                            value |= bitstream_reader_read_bit(&reader) << i;
                        }
                        v = static_cast<double>(value);
                        break;
                    }
                    }
                } else {
                    // little endian
                    switch (bs.size) {
                        case 1:
                        v = static_cast<double>(bitstream_reader_read_bit(&reader)); break;
                    case 8:
                        if (signal.is_signed)
                            v = static_cast<double>(static_cast<int8_t>(bitstream_reader_read_u8(&reader)));
                        else
                            v = static_cast<double>(bitstream_reader_read_u8(&reader));
                        break;
                    case 16:
                        if (signal.is_signed)
                            v = static_cast<double>(static_cast<int16_t>(bitstream_reader_read_u16(&reader)));
                        else
                            v = static_cast<double>(bitstream_reader_read_u16(&reader));
                        break;
                    case 32:
                        if (signal.is_signed)
                            v = static_cast<double>(static_cast<int32_t>(bitstream_reader_read_u32(&reader)));
                        else
                            v = static_cast<double>(bitstream_reader_read_u32(&reader));
                        break;
                    case 64:
                        if (signal.is_signed)
                            v = static_cast<double>(static_cast<int64_t>(bitstream_reader_read_u64(&reader)));
                        else
                            v = static_cast<double>(bitstream_reader_read_u64(&reader));
                        break;
                    default: {
                        // TODO: possible to implement bigendian and sign?
                        uint64_t value = 0;
                        for (uint32_t i=0; i < bs.size; i++) {
                            value |= bitstream_reader_read_bit(&reader) << i;
                        }
                        v = static_cast<double>(value);
                        break;
                    }
                    }
                }

                values.push_back(v * signal.factor + signal.offset);
            }
        }
        return true;
    }

    void Message::appendSignal(const Signal& signal) {
        m_prepared = false;
        m_signals.push_back(signal);
    }

    const std::vector<Signal> Message::signals() const {
       return m_signals;
    }

    uint32_t Message::id() const {
        return m_id;
    }

    void Message::prepareMessage() {
        m_prepared = false;
        // sort signals so that the signals are ordered by the startbit
        std::sort(m_signals.begin(), m_signals.end());

        uint32_t curr_bit = 0;
        for (std::vector<Signal>::size_type i=0; i < m_signals.size(); i++) {
            const auto& signal = m_signals.at(i);
            if (signal.is_multiplexed)
                break; // Not supported yet

            if (curr_bit < signal.start_bit) {
                // padding needed
                bitstruct.push_back(BitStruct(signal.start_bit - curr_bit));
            }
            bitstruct.push_back(BitStruct(i, signal.size));
            curr_bit = signal.start_bit + signal.size;
        }
        // Check if correct
        if (curr_bit > m_size * 8)
            return;

        m_prepared = true;
    }

    std::ostream& operator<< (std::ostream &out, const Message& msg) {
        out << "Message: {id: " << msg.id() << ", ";
        out << "name: " << msg.m_name << ", ";
        out << "size: " << msg.m_size << ", ";
        out << "node: " << msg.m_node << "}";
		return out;
	}
}
