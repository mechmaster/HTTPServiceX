#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include <iostream>

namespace cassandra
{
    namespace types
    {
        // Versions
        const std::uint8_t VER_REQUEST_1 = 0x01; // v1
        const std::uint8_t VER_REQUEST_2 = 0x02; // v2
        const std::uint8_t VER_REQUEST_3 = 0x03; // v3
        const std::uint8_t VER_REQUEST_4 = 0x04; // v4
        const std::uint8_t VER_REQUEST_5 = 0x05; // v5 beta
        const std::uint8_t VER_REQUEST_DSE_1 = 0x41; // Only for Datastax DB Enterprice
        const std::uint8_t VER_REQUEST_DSE_2 = 0x42; // Only for Datastax DB Enterprice
        
        const std::uint8_t VER_RESPONSE_1 = 0x81; // v1
        const std::uint8_t VER_RESPONSE_2 = 0x82; // v2
        const std::uint8_t VER_RESPONSE_3 = 0x83; // v3
        const std::uint8_t VER_RESPONSE_4 = 0x84; // v4
        const std::uint8_t VER_RESPONSE_5 = 0x85; // v5 beta
        const std::uint8_t VER_RESPONSE_DSE_1 = 0xC1; // Only for Datastax DB Enterprice
        const std::uint8_t VER_RESPONSE_DSE_2 = 0xC2; // Only for Datastax DB Enterprice

        // Flags
        const std::uint8_t FLAG_COMPRESSION = 0x01;
        const std::uint8_t FLAG_TRACING = 0x02;
        const std::uint8_t FLAG_CUSTOM_PAYLOAD = 0x04;
        const std::uint8_t FLAG_WARNING = 0x08;
        const std::uint8_t FLAG_USE_BETA = 0x10; // Use only protocol version 5.

        // Opcodes
        const std::uint8_t OP_ERROR = 0x00; // ERROR
        const std::uint8_t OP_STARTUP = 0x01; // STARTUP
        const std::uint8_t OP_READY = 0x02; // READY
        const std::uint8_t OP_AUTHENTICATE = 0x03; // AUTHENTICATE
        const std::uint8_t OP_OPTIONS = 0x05; // OPTIONS
        const std::uint8_t OP_SUPPORTED = 0x06; // SUPPORTED
        const std::uint8_t OP_QUERY = 0x07; // QUERY
        const std::uint8_t OP_RESULT = 0x08; // RESULT
        const std::uint8_t OP_PREPARE = 0x09; // PREPARE
        const std::uint8_t OP_EXECUTE = 0x0A; // EXECUTE
        const std::uint8_t OP_REGISTER = 0x0B; // REGISTER
        const std::uint8_t OP_EVENT = 0x0C; // EVENT
        const std::uint8_t OP_BATCH = 0x0D; // BATCH
        const std::uint8_t OP_AUTH_CHALLENGE = 0x0E; // AUTH_CHALLENGE
        const std::uint8_t OP_AUTH_RESPONSE = 0x0F; // AUTH_RESPONSE
        const std::uint8_t OP_AUTH_SUCCESS = 0x10; // AUTH_SUCCESS

        const std::string STARTUP_CQL_VERSION = "CQL_VERSION";
        const std::string STARTUP_CQL_VERSION_VALUE = "3.0.0";

        const std::int32_t BEGIN_BODE_INDEX = 9;
        const std::uint32_t HEADER_LENGTH = 9;

        typedef std::vector<std::uint8_t> Buffer_t;

        struct PacketHeader
        {
            std::uint8_t version_;
            std::uint8_t flags_;
            std::int16_t id_;
            std::uint8_t opcode_;
            std::uint32_t bodyLength_;
            
            bool use_compression_;

            PacketHeader()
            {
                version_ = 0;
                flags_ = 0;
                id_ = 0;
                opcode_ = 0;
                bodyLength_ = 0;
                use_compression_ = false;
            }

            void print()
            {
                std::cout << "Begin packet header." << std::endl;
                if (version_ == types::VER_REQUEST_4 || version_ == types::VER_REQUEST_5)
                {
                    std::cout << "query type = Request;" << std::endl;
                }
                if (version_ == types::VER_RESPONSE_4 || version_ == types::VER_RESPONSE_5)
                {
                    std::cout << "query type = Response;" << std::endl;
                }
                std::cout << "version = " << static_cast<std::uint16_t>(version_) << ";" << std::endl;
                std::cout << "flags = " << static_cast<std::uint16_t>(flags_) << ";" << std::endl;
                std::cout << "id = " << id_ << ";" << std::endl;
                std::cout << "opcode = " << static_cast<std::uint16_t>(opcode_) << ";" << std::endl;
                std::cout << "bodyLength = " << bodyLength_ << ";" << std::endl;
                std::cout << "use_compression = " << static_cast<std::uint16_t>(use_compression_) << ";" << std::endl;
                std::cout << "End packet header." << std::endl;
                std::cout << std::endl;
            }
        };

        struct CellIndex
        {
            std::int32_t row_;
            std::int32_t column_;

            CellIndex()
            {
                row_ = -1;
                column_ = -1;
            }
            
            bool isValid()
            {
                return row_ != -1 && column_ != -1;
            }
            
            bool operator < (const CellIndex& index)
            {
              if (row_ < index.row_)
              {
                return true;
              }

              if (row_ == index.row_ && column_ < index.column_)
              {
                return true;
              }

              return false;
            }
            
            bool operator == (const CellIndex& index)
            {
              return row_ == index.row_ && column_ == index.column_;
            }
        };
        
        inline bool operator < (const cassandra::types::CellIndex& a, const cassandra::types::CellIndex& b)
        {
            if (a.row_ < b.row_)
            {
                return true;
            }

            if (a.row_ == b.row_ && a.column_ < b.column_)
            {
                return true;
            }

            return false;
        }

        struct DataTypeInfo
        {
            std::uint16_t type_;
            std::vector<std::uint16_t> subTypesList_;

            DataTypeInfo()
            {
                type_ = 0;
            }

            bool empty()
            {
                return (type_ == 0) && subTypesList_.empty();
            }
        };

        struct CellMetaInfo
        {
            CellIndex index_;
            std::string name_;
            DataTypeInfo type_;
            
            CellMetaInfo()
            {
                type_.type_ = 0;
            }
        };
        
        struct TempCellData
        {
            std::int32_t currentRow_;
            std::int32_t currentColomn_;
            std::size_t savedBytes_;
            std::size_t cellDataSize_;
            unsigned int index_;
            std::int16_t count_;
            bool needReadCount_;
            std::vector<std::uint8_t> data;
            
            TempCellData()
            {
                currentRow_ = 0;
                currentColomn_ = 0;
                savedBytes_ = 0;
                cellDataSize_ = 0;
                index_ = 0;
                count_ = 0;
                needReadCount_ = true;
            }
        };

        struct PacketInfo
        {
            std::int16_t version_;
            bool compression_;
            bool tracing_;
            bool custom_payload;
            bool warning_;
            bool use_beta_;
            bool use_lz4_;
            bool use_snappy_;
            std::uint16_t id_;
            bool request_;
            bool response_;
            std::string text_;
            std::int32_t columns_; // column count;
            std::int32_t rows_; // row count;
            bool isQuery_;
            bool isError_;
            std::map<types::CellIndex, types::CellMetaInfo> schema_;
            std::map<types::CellIndex, std::vector<std::uint8_t>> valuesList_;

            std::uint16_t errorCode_;
            bool isPackedParsed_;

            PacketInfo()
            {
                version_ = -1;
                compression_ = false;
                tracing_ = false;
                custom_payload = false;
                warning_ = false;
                use_beta_ = false;
                use_lz4_ = false;
                use_snappy_ = false;
                id_ = 0;
                request_ = false;
                response_ = false;
                columns_ = 0; // column count;
                rows_ = 0; // row count;
                isQuery_ = false;
                isError_ = false;
                errorCode_ = 0;
                isPackedParsed_ = false;
            }

            void clear()
            {
                version_ = -1;
                compression_ = false;
                tracing_ = false;
                custom_payload = false;
                warning_ = false;
                use_beta_ = false;
                use_lz4_ = false;
                use_snappy_ = false;
                id_ = 0;
                request_ = false;
                response_ = false;
                columns_ = 0; // column count;
                rows_ = 0; // row count;
                isQuery_ = false;
                isError_ = false;
                errorCode_ = 0;
                isPackedParsed_ = false;

                if (!text_.empty())
                {
                    text_.clear();
                }

                if (!schema_.empty())
                {
                    schema_.clear();
                }

                if (!valuesList_.empty())
                {
                    valuesList_.clear();
                }
            }
        };

        typedef std::vector<PacketInfo> PacketInfoList_t;
    }

    enum class ParserStatus
    {
        PARSING_IN_PROGRESS,
        PARSING_IS_DONE
    };
}
