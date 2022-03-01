#include <map>

#define CRCPP_INCLUDE_ESOTERIC_CRC_DEFINITIONS
#include "CRC.h"

#include "CassandraTypes.h"
#include "CassandraUtils.h"

namespace cassandra
{
    namespace utils
    {
        std::string getString(const char* buffer, std::size_t size, int& index)
        {
            if (index >= size)
            {
                return {};
            }

            std::int16_t length = getShort(buffer, size, index);
            if (length == 0)
            {
                return {};
            }

            std::string value;
            value.resize(length);

            for (int i = 0; i < length; ++i)
            {
                value[i] = buffer[index];
                ++index;
            }

            return value;
        }

        std::string getLongString(const char* buffer, std::size_t size, int& index)
        {
            if (index >= size)
            {
                return {};
            }

            std::int32_t length = getInt(buffer, size, index);
            if (length == 0)
            {
                return {};
            }

            std::string value;
            value.resize(length);

            for (int i = 0; i < length; ++i)
            {
                value[i] = buffer[index];
                ++index;
            }

            return value;
        }

        void setString(char* buffer, std::size_t size, int& index, const std::string& text)
        {
            if (index >= size)
            {
                return;
            }

            std::int16_t length = static_cast<std::int16_t>(text.size());
            setShort(buffer, size, index, length);

            for (unsigned int i = 0; i < text.size(); ++i)
            {
                buffer[index] = text[i];
                ++index;
            }
        }

        void setLongString(char* buffer, std::size_t size, int& index, const std::string& text)
        {
            if (index >= size)
            {
                return;
            }

            std::int32_t length = static_cast<std::int32_t>(text.size());
            setInt(buffer, size, index, length);
            
            for (unsigned int i = 0; i < text.size(); ++i)
            {   
                buffer[index] = text[i];
                ++index;
            }
        }

        std::int32_t getInt(const char* buffer, std::size_t size, int& index)
        {
            std::int32_t value = 0;
            
            if (index >= size)
            {
                return value;
            }

            value |= (static_cast<std::uint8_t>(buffer[index]) << 24); ++index;
            value |= (static_cast<std::uint8_t>(buffer[index]) << 16); ++index;
            value |= (static_cast<std::uint8_t>(buffer[index]) << 8); ++index;
            value |= static_cast<std::uint8_t>(buffer[index]); ++index;

            return value;
        }

        std::int16_t getShort(const char* buffer, std::size_t size, int& index)
        {
            std::int16_t value = 0;

            if (index >= size)
            {
                return value;
            }

            value |= (static_cast<std::uint8_t>(buffer[index]) << 8); ++index;
            value |= static_cast<std::uint8_t>(buffer[index]); ++index;

            return value;
        }

        void setShort(char* buffer, std::size_t size, int& index, const std::int16_t& value)
        {
            if (index >= size)
            {
                return;
            }
            
            buffer[index] = static_cast<std::uint8_t>((value >> 8)); ++index;
            buffer[index] = static_cast<std::uint8_t>(value); ++index;
        }

        void setInt(char* buffer, std::size_t size, int& index, const std::int32_t& value)
        {
            if (index >= size)
            {
                return;
            }

            buffer[index] = static_cast<std::uint8_t>((value >> 24)); ++index;
            buffer[index] = static_cast<std::uint8_t>(value >> 16); ++index;
            buffer[index] = static_cast<std::uint8_t>((value >> 8)); ++index;
            buffer[index] = static_cast<std::uint8_t>(value); ++index;        
        }

        std::int16_t detectProtocolVersion(const uint8_t verByte)
        {
            static std::map<std::uint8_t, std::uint16_t> supportVersionByte
            {
                {types::VER_REQUEST_1, (uint16_t)1},
                {types::VER_REQUEST_2, (uint16_t)2},
                {types::VER_REQUEST_3, (uint16_t)3},
                {types::VER_REQUEST_4, (uint16_t)4},
                {types::VER_REQUEST_5, (uint16_t)5},
                {types::VER_REQUEST_DSE_1, (uint16_t)65},
                {types::VER_REQUEST_DSE_2, (uint16_t)66},
                
                {types::VER_RESPONSE_1, (uint16_t)1},
                {types::VER_RESPONSE_2, (uint16_t)2},
                {types::VER_RESPONSE_3, (uint16_t)3},
                {types::VER_RESPONSE_4, (uint16_t)4},
                {types::VER_RESPONSE_5, (uint16_t)5},
                {types::VER_RESPONSE_DSE_1, (uint16_t)65},
                {types::VER_RESPONSE_DSE_2, (uint16_t)66},
            };

            auto iter = supportVersionByte.find(verByte);
            if (iter == supportVersionByte.end())
            {
                return -1;
            }
            
            return iter->second;
        }

        bool getOffsetAndVersion(const char* buffer, std::size_t bufferSize, std::int16_t& version,
            std::uint8_t& offset, bool useOffset)
        {
            if (bufferSize < 9)
            {
                version = -1;
                offset = 0;
                return false;
            }

            if (!useOffset)
            {
                version = detectProtocolVersion(buffer[0]);
                offset = 0;

                return version != -1;
            }

            version = detectProtocolVersion(buffer[6]);
            if (version != -1)
            {
                offset = 6;
            }
            else
            {
                version = detectProtocolVersion(buffer[8]);
                if (version != -1)
                {
                    offset = 8;
                }
                else
                {
                    version = detectProtocolVersion(buffer[0]);
                    if (version != -1)
                    {
                        offset = 0;
                    }
                }
            }

            return version != -1;
        }
        
        std::uint32_t calculateCRC24(const char* data, std::size_t dataSize)
        {
            std::uint32_t crc = CRC::Calculate(data, dataSize, CRC::CRC_24_CASSANDRA());
            return crc;
        }

        std::uint32_t calculateCRC32(const char* data, std::size_t dataSize)
        {
            static std::vector<unsigned char> initialBytes = {0xFA, 0x2D, 0x55, 0xCA}; // Magic value. Use for initial crc value.

            std::uint32_t crc = 0;
            crc = CRC::Calculate(&initialBytes[0], initialBytes.size(), CRC::CRC_32());
            crc = CRC::Calculate(data, dataSize, CRC::CRC_32(), crc);
            return crc;
        }
    }
}
