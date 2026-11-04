#include "crsf.h"
#include "crsf_callbacks.h"
#include "packet_catcher.h"

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <exception>
#include <iostream>
#include <string>

namespace {

constexpr size_t kChannelSlots = 32;

const char* packetTypeName(uint8_t type) {
    switch (static_cast<crsf::PacketType>(type)) {
        case crsf::PacketType::GPS: return "GPS";
        case crsf::PacketType::Vario: return "Vario";
        case crsf::PacketType::BatterySensor: return "BatterySensor";
        case crsf::PacketType::BaroAltitude: return "BaroAltitude";
        case crsf::PacketType::Heartbeat: return "Heartbeat";
        case crsf::PacketType::LinkStatistics: return "LinkStatistics";
        case crsf::PacketType::RCChannelsPacked: return "RCChannelsPacked";
        case crsf::PacketType::SubsetRCChannelsPacked: return "SubsetRCChannelsPacked";
        case crsf::PacketType::LinkRXID: return "LinkRXID";
        case crsf::PacketType::LinkTXID: return "LinkTXID";
        case crsf::PacketType::Attitude: return "Attitude";
        case crsf::PacketType::FlightMode: return "FlightMode";
        case crsf::PacketType::DevicePing: return "DevicePing";
        case crsf::PacketType::DeviceInfo: return "DeviceInfo";
        case crsf::PacketType::ParameterSettingsEntry: return "ParameterSettingsEntry";
        case crsf::PacketType::ParameterRead: return "ParameterRead";
        case crsf::PacketType::ParameterWrite: return "ParameterWrite";
        case crsf::PacketType::ELRSStatus: return "ELRSStatus";
        case crsf::PacketType::Command: return "Command";
        case crsf::PacketType::Data: return "Data";
        case crsf::PacketType::RadioID: return "RadioID";
        case crsf::PacketType::MSPReq: return "MSPReq";
    }
    return nullptr;
}

class Viewer {
public:
    void onPacket(const uint8_t* data, size_t size) {
        if (size < crsf::MIN_PACKET_SIZE || size > crsf::MAX_PACKET_SIZE) {
            return;
        }

        std::array<uint8_t, crsf::MAX_PACKET_SIZE> raw{};
        std::memcpy(raw.data(), data, size);
        etl::span<uint8_t> span(raw.data(), size);
        const auto type = static_cast<crsf::PacketType>(data[2]);

        if (type == crsf::PacketType::RCChannelsPacked) {
            crsf::CRSFRCChannels packet(span);
            std::string line;
            for (size_t i = 0; i < crsf::CRSFRCChannels::CHANNEL_COUNT; ++i) {
                noteChannel(line, i, packet.getChannel(static_cast<uint8_t>(i)));
            }
            writeLine(line);
            return;
        }

        if (type == crsf::PacketType::SubsetRCChannelsPacked) {
            crsf::CRSFSubsetRCChannels packet(span);
            const size_t first = packet.getFirstChannelNumber();
            const size_t count = packet.getChannelCount();
            std::string line;
            for (size_t i = 0; i < count && first + i < kChannelSlots; ++i) {
                noteChannel(line, first + i, packet.getChannel(static_cast<uint8_t>(i)));
            }
            writeLine(line);
            return;
        }

        const uint64_t count = ++otherCounts_[data[2]];
        const char* name = packetTypeName(data[2]);
        if (name != nullptr) {
            std::cout << name << ' ' << count << std::endl;
        } else {
            std::cout << "0x" << std::hex << static_cast<unsigned>(data[2]) << std::dec
                      << ' ' << count << std::endl;
        }
    }

private:
    void noteChannel(std::string& line, size_t index, uint16_t value) {
        char piece[64];
        if (!seen_[index]) {
            seen_[index] = true;
            channels_[index] = value;
            std::snprintf(piece, sizeof(piece), "ch%zu=%u", index + 1, static_cast<unsigned>(value));
        } else if (channels_[index] != value) {
            std::snprintf(piece, sizeof(piece), "ch%zu %u -> %u", index + 1,
                          static_cast<unsigned>(channels_[index]), static_cast<unsigned>(value));
            channels_[index] = value;
        } else {
            return;
        }
        if (!line.empty()) {
            line.push_back(' ');
        }
        line += piece;
    }

    static void writeLine(const std::string& line) {
        if (!line.empty()) {
            std::cout << line << std::endl;
        }
    }

    std::array<uint16_t, kChannelSlots> channels_{};
    std::array<bool, kChannelSlots> seen_{};
    std::array<uint64_t, 256> otherCounts_{};
};

} // namespace

int main(int argc, char** argv) {
    if (argc > 2) {
        std::fprintf(stderr, "usage: crsf-stream-viewer [file]\n");
        return 1;
    }

    FILE* in = stdin;
    if (argc == 2) {
        in = std::fopen(argv[1], "rb");
        if (in == nullptr) {
            std::perror(argv[1]);
            return 1;
        }
    }

    Viewer viewer;
    PacketCatcher catcher;
    ProtocolCallbacks callbacks;
    callbacks.isPacketHead = crsf::crsfIsPacketHead;
    callbacks.isValidPacket = crsf::crsfIsValidPacket;
    callbacks.getTotalPacketSize = crsf::crsfGetTotalPacketSize;
    callbacks.processValidPacket = [&viewer](const uint8_t* data, size_t size) {
        try {
            viewer.onPacket(data, size);
        } catch (const std::exception&) {
        }
    };
    catcher.setProtocolCallbacks(callbacks);

    std::array<uint8_t, 512> buffer{};
    while (true) {
        const size_t n = std::fread(buffer.data(), 1, buffer.size(), in);
        if (n > 0) {
            catcher.processBytes(buffer.data(), n);
        }
        if (n < buffer.size()) {
            break;
        }
    }

    const bool readError = std::ferror(in) != 0;
    if (in != stdin) {
        std::fclose(in);
    }
    if (readError) {
        std::perror("read");
        return 1;
    }
    return 0;
}
