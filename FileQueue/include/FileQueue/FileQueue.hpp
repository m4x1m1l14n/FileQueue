#pragma once

#include <string>
#include <vector>
#include <fstream>

namespace m4x1m1l14n
{
    namespace details
    {
        class FileQueueRecord
        {
        public:
            FileQueueRecord(const std::string& data)
                : m_data(data)
            {

            }

        protected:
            std::uint32_t m_previousRecordOffset;
            std::uint32_t m_nextRecordOffset;
            std::string m_data;
        };

        class FileQueueHeader
        {
        public:

        };
    }


    class FileQueue
    {
    public:
        const size_t FileQueueBlockSize = 4096;

    public:
        FileQueue(const std::string& path)
            : FileQueue(path, false)
        {

        }

        FileQueue(const std::string& path, bool overwrite)
            : m_path(path)
            , m_previousRecordOffset(0)
        {
            if (path.empty())
            {
                throw std::invalid_argument("path cannot be empty!");
            }

            auto mode = std::fstream::out | std::fstream::in | std::fstream::binary;
            mode |= (overwrite) 
                ? std::fstream::trunc 
                : std::fstream::app;

            m_file.open(m_path, mode);
            if (!m_file.is_open())
            {
                throw std::runtime_error("Cannot open file \"" + path + "\"");
            }
        }

        virtual ~FileQueue()
        {
            this->Flush();
        }

        void Push(const std::vector<std::string>& data)
        {
            for (const auto& x : data)
            {
                this->Push(x);
            }
        }

        void Push(const std::string& data)
        {
            const auto& record = details::FileQueueRecord(data);

            const std::uint32_t dataLength = static_cast<std::uint32_t>(data.length());
            const std::uint32_t recordLength = 
                static_cast<std::uint32_t>(sizeof(m_previousRecordOffset)) + 
                static_cast<std::uint32_t>(sizeof(dataLength)) + 
                dataLength;

            m_file.write(reinterpret_cast<const char*>(&dataLength), sizeof(dataLength));
            m_file.write(data.c_str(), data.length());
            m_file.write(reinterpret_cast<const char*>(&m_previousRecordOffset), sizeof(m_previousRecordOffset));

            m_previousRecordOffset += (sizeof(dataLength) + dataLength + sizeof(m_previousRecordOffset));

            /*std::streamoff pos = m_file.tellp();
            std::streamoff newpos = pos - dataLength;

            m_file.seekg(newpos);*/

            /*const std::uint32_t recordLength = sizeof(std::uint32_t) + sizeof(std::uint32_t) + dataLength;

            std::uint32_t previousRecordLength = 0;
            std::uint32_t nextRecordOffset = dataLength;

            std::string record;

            record.reserve(recordLength);
            record.append(reinterpret_cast<const char*>(&previousRecordLength), sizeof(previousRecordLength));
            record.append(reinterpret_cast<const char*>(&nextRecordOffset), sizeof(nextRecordOffset));
            record.append(data.c_str(), data.length());
            record.append(reinterpret_cast<const char*>(&nextRecordOffset), sizeof(nextRecordOffset));

            m_file.write(record.c_str(), record.length());*/
        }

        std::string Pop()
        {
            std::streamoff pos = m_file.tellg();

            m_file.seekg(pos - 4);

            std::uint32_t previousRecordOffset = 0;

            m_file.read(reinterpret_cast<char*>(&previousRecordOffset), sizeof(previousRecordOffset));

            std::streamoff previousDataOffset = previousRecordOffset + sizeof(std::uint32_t);

            m_file.seekg(previousDataOffset);

            size_t recordDataLength = pos - previousRecordOffset - 2 * sizeof(std::uint32_t);

            auto buffer = std::make_unique<char[]>(recordDataLength);

            m_file.read(buffer.get(), recordDataLength);

            return std::string(buffer.get(), recordDataLength);
        }

        std::vector<std::string> Pop(const size_t count)
        {
            std::vector<std::string> result;

            for (size_t i = 0; i < count; ++i)
            {
                const auto& data = this->Pop();
                if (data.empty())
                {
                    break;
                }

                result.push_back(data);
            }

            return result;
        }

        void Clear()
        {

        }

        size_t Size()
        {
            const auto backup = m_file.tellg();
            m_file.seekg(0, m_file.end);
            const auto size = m_file.tellg();
            m_file.seekg(backup);

            return static_cast<size_t>(size);
        }

        size_t Length()
        {
            throw std::runtime_error("not implemented");
        }

        void Flush()
        {
            m_file.flush();
        }

        bool Empty()
        {
            return (this->Length() == 0);
        }

    protected:
        void PushRecord(const details::FileQueueRecord& record)
        {

        }

    protected:
        std::string m_path;
        std::string m_block;
        std::uint32_t m_previousRecordOffset;
        std::uint32_t m_nextRecord;
        std::fstream m_file;
    };
}
