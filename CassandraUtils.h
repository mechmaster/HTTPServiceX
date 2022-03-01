#pragma once

#include <cstdint>
#include <string>

#include "CassandraTypes.h"

namespace cassandra
{ 
    namespace utils
    { 
        std::string getString(const char* buffer, std::size_t size, int& index);
        std::string getLongString(const char* buffer, std::size_t size, int& index);
        std::int32_t getInt(const char* buffer, std::size_t size, int& index);
        std::int16_t getShort(const char* buffer, std::size_t size, int& index);
        
        void setString(char* buffer, std::size_t size, int& index, const std::string& text);
        void setLongString(char* buffer, std::size_t size, int& index, const std::string& text);
        void setShort(char* buffer, std::size_t size, int& index, const std::int16_t& value);
        void setInt(char* buffer, std::size_t size, int& index, const std::int32_t& value);

        std::int16_t detectProtocolVersion(const std::uint8_t verByte);
        bool getOffsetAndVersion(const char* buffer, std::size_t bufferSize, std::int16_t& version,
            std::uint8_t& offset, bool useOffset = true);
        std::uint32_t calculateCRC24(const char* data, std::size_t dataSize);
        std::uint32_t calculateCRC32(const char* data, std::size_t dataSize);
    }
}
