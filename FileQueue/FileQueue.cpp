// FileQueue.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>


namespace fs = std::filesystem;


namespace m4x1m1l14n
{
    namespace details
    {
        class FileQueueRecord
        {

        public:
            std::uint32_t m_previousRecordOffset;
            std::uint32_t m_nextRecordOffset;
            std::uint32_t m_data;
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
            : m_path(path)
        {
            if (path.empty())
            {
                throw std::invalid_argument("path cannot be empty!");
            }

            m_file.open(m_path, std::ios::out | std::ios::binary);
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
            const std::uint32_t dataLength = static_cast<std::uint32_t>(data.length());
            const std::uint32_t recordLength = sizeof(std::uint32_t) + sizeof(std::uint32_t) + dataLength;

            std::uint32_t previousRecordLength = 0;
            std::uint32_t nextRecordOffset = dataLength;

            std::string record;

            record.reserve(recordLength);
            record.append(reinterpret_cast<const char*>(&previousRecordLength), sizeof(previousRecordLength));
            record.append(reinterpret_cast<const char*>(&nextRecordOffset), sizeof(nextRecordOffset));
            record.append(data.c_str(), data.length());
            record.append(reinterpret_cast<const char*>(&nextRecordOffset), sizeof(nextRecordOffset));

            m_file.write(record.c_str(), record.length());
        }

        std::string Pop()
        {
            return "";
        }

        std::vector<std::string> Pop(const size_t count)
        {
            std::vector<std::string> result;

            for (size_t i = 0; i < count; ++i)
            {
                const auto& data = this->Pop();

                result.push_back(data);
            }

            return result;
        }

        void Flush()
        {
            m_file.flush();
        }

    protected:
        void PushRecord(const details::FileQueueRecord& record)
        {

        }

    protected:
        std::string m_path;
        std::string m_block;
        std::uint32_t m_previousRecord;
        std::uint32_t m_nextRecord;
        std::ofstream m_file;
    };
}


using namespace m4x1m1l14n;


int main()
{
    auto fifo = std::make_unique<FileQueue>("queue.cache");

    fifo->Push("Hello");
    fifo->Push("world");
    fifo->Push("world");
    fifo->Pop();
    fifo->Push("Mistake!");
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
