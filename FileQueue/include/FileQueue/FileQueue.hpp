#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cassert>


// TODO Max items count (infinite|excplicitly set)

namespace fs = std::filesystem;


namespace m4x1m1l14n
{
	typedef std::int32_t RecordOffset;

	constexpr const RecordOffset ZeroOffset = 0;
	constexpr std::int32_t RecordOffsetSize = sizeof(RecordOffset);
	constexpr std::int32_t FileQueueHeaderSize = 0;
	constexpr std::uint32_t FileListHeaderSize = 1024;


	namespace impl
	{
		class FileListHeader
		{
		public:
			FileListHeader()
			{

			}

			FileListHeader(const std::string& data)
			{

			}

			static FileListHeader Create()
			{
				return FileListHeader();
			}
		};

		class FileListRecord
		{
		public:
			FileListRecord()
			{

			}

			FileListRecord(const std::unique_ptr<char[]>& buffer, const size_t length)
			{
				if (!buffer)
				{
					throw std::invalid_argument("buffer cannot be null");
				}

				assert(length >= sizeof(RecordOffset));

				m_nextRecordOffset = *reinterpret_cast<const RecordOffset*>(buffer.get());
				// m_previousRecordOffset = *reinterpret_cast<const RecordOffset*>(buffer.c_str() + (buffer.length() - sizeof(RecordOffset)));

				m_data = std::string(buffer.get() + sizeof(RecordOffset), length - sizeof(RecordOffset));
			}

			const std::string& Data() const
			{
				return m_data;
			}

			std::string Data()
			{
				return m_data;
			}

		protected:
			// RecordOffset m_previousRecordOffset;
			RecordOffset m_nextRecordOffset;
			std::string m_data;
		};

		typedef std::shared_ptr<FileListRecord> FileListRecord_ptr;
	}


	/**
	 * File-based double ended queue implementation
	 */
	class FileList
	{
	public:
		FileList(const std::string& path)
			: FileList(path, false)
		{

		}

		FileList(const std::string& path, bool overwrite)
			: m_length(0)
		{
			if (path.empty())
			{
				throw std::invalid_argument("'path' cannot be empty!");
			}

			const auto exists = fs::exists(path);
			if (!exists || overwrite)
			{
				this->CreateFile(path);
			}
			else
			{
				this->ReOpenFile(path);
			}
		}

		virtual ~FileList()
		{
			this->Flush();
		}

		void PushBack(const std::string& data)
		{
			const auto dataLength = static_cast<RecordOffset>(data.length());

			const auto nextRecordOffset = static_cast<RecordOffset>(sizeof(RecordOffset) + dataLength + sizeof(RecordOffset));
			const auto previousRecordOffset = -nextRecordOffset;

			m_file.write(reinterpret_cast<const char*>(&nextRecordOffset), sizeof(nextRecordOffset));
			m_file.write(data.c_str(), data.length());
			m_file.write(reinterpret_cast<const char*>(&previousRecordOffset), sizeof(previousRecordOffset));

			++m_length;
		}

		std::string PopBack()
		{
			const auto previousRecordOffset = this->GetPreviousRecordOffset();
			if (previousRecordOffset == 0)
			{
				return std::string();
			}

			m_file.seekg(previousRecordOffset, std::ios::cur);

			const auto bytesToRead = std::abs(previousRecordOffset) - sizeof(RecordOffset);

			auto buffer = std::make_unique<char[]>(bytesToRead);

			m_file.read(buffer.get(), bytesToRead);

			const auto bytesRead = m_file.gcount();

			assert(bytesRead == bytesToRead);

			m_file.seekg(-bytesRead, std::ios::cur);

			m_file.write(reinterpret_cast<const char*>(&ZeroOffset), sizeof(ZeroOffset));

			m_file.seekg(-RecordOffsetSize, std::ios::cur);

			--m_length;

			const impl::FileListRecord record(buffer, bytesRead);

			const auto& result = record.Data();

			return result;
		}

		void PushFront(const std::string& data)
		{
			throw std::runtime_error("not implemented");
		}

		std::string PopFront()
		{
			throw std::runtime_error("not implemented");
		}

		// std::string Front();
		// std::string Back();

		void Flush()
		{
			m_file.flush();
		}

		void Clear()
		{

		}

		size_t Size()
		{
			const auto position = m_file.tellg();
			
			m_file.seekg(0, m_file.end);

			const auto size = m_file.tellg();
			
			m_file.seekg(position);

			return static_cast<size_t>(size);
		}

		size_t Length()
		{
			return m_length;
		}

		bool Empty()
		{
			return (this->Length() == 0);
		}

		virtual void Push(const std::string& data)
		{
			throw std::runtime_error("not implemented");
		}

		void Push(const std::vector<std::string>& data)
		{
			for (const auto& x : data)
			{
				this->Push(x);
			}
		}

		virtual std::string Pop()
		{
			throw std::runtime_error("not implemented");
		}

		std::vector<std::string> Pop(size_t count)
		{
			std::vector<std::string> result;

			while (count--)
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

	protected:
		void CreateFile(const std::string& path)
		{
			const auto mode = std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::trunc;

			m_file.open(path, mode);
			if (!m_file.is_open())
			{
				throw std::runtime_error("Cannot open file \"" + path + "\"");
			}
		}

		void ReOpenFile(const std::string& path)
		{
			const auto mode = std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::app;

			m_file.open(path, mode);
			if (!m_file.is_open())
			{
				throw std::runtime_error("Cannot open file \"" + path + "\"");
			}

			m_header = this->LoadHeader();

			
		}

		impl::FileListHeader LoadHeader()
		{
			const auto position = m_file.tellg();
			if (position != 0)
			{
				m_file.seekg(0);
			}

			char buffer[FileListHeaderSize];

			m_file.read(buffer, std::size(buffer));

			const auto bytesRead = m_file.gcount();
			
			m_file.seekg(position);

			const auto& data = std::string(buffer, bytesRead);

			return impl::FileListHeader(data);
		}

		void SaveHeader(const impl::FileListHeader& header)
		{
			char buffer[FileListHeaderSize] = { 0 };

			const auto position = m_file.tellg();
			if (position != 0)
			{
				m_file.seekg(0);
			}

			m_file.write(buffer, std::size(buffer));

			const auto bytesWritten = m_file.gcount();

			m_file.seekg(position);

			assert(bytesWritten == std::size(buffer));
		}

		RecordOffset GetPreviousRecordOffset()
		{
			RecordOffset previousRecordOffset = 0;

			const auto position = m_file.tellg();
			if (position > FileQueueHeaderSize)
			{
				const auto offset = static_cast<std::streamoff>(sizeof(RecordOffset));

				m_file.seekg(-offset, std::ios::cur);

				m_file.read(reinterpret_cast<char*>(&previousRecordOffset), sizeof(previousRecordOffset));

				const auto bytesRead = m_file.gcount();

				assert(bytesRead == sizeof(RecordOffset));
			}

			return previousRecordOffset;
		}

		impl::FileListRecord_ptr GetPreviousRecord()
		{
			return std::make_shared<impl::FileListRecord>();
		}

		RecordOffset GetNextRecordOffset()
		{
			RecordOffset nextRecordOffset = 0;

			const auto position = m_file.tellg();

			return nextRecordOffset;
		}

	protected:
		std::fstream m_file;
		impl::FileListHeader m_header;
		std::uint64_t m_length;
	};

	typedef std::shared_ptr<FileList> FileList_ptr;


	/**
	 * LIFO implementation
	 */
	class FileStack
		: public FileList
	{
	public:
		FileStack(const std::string& path)
			: FileList(path)
		{

		}

		FileStack(const std::string& path, bool overwrite)
			: FileList(path, overwrite)
		{

		}

		virtual ~FileStack()
		{

		}

		virtual void Push(const std::string& data) override
		{
			this->PushBack(data);
		}

		virtual std::string Pop() override
		{
			return this->PopBack();
		}
	};

	typedef std::shared_ptr<FileStack> FileStack_ptr;


	/**
	 * FIFO implementation
	 */
	class FileQueue
		: public FileList
	{
	public:
		FileQueue(const std::string& path)
			: FileList(path)
		{

		}

		FileQueue(const std::string& path, bool overwrite)
			: FileList(path, overwrite)
		{

		}

		virtual ~FileQueue()
		{
			
		}

		virtual void Push(const std::string& data) override
		{
			this->PushBack(data);
		}

		virtual std::string Pop() override
		{
			return this->PopFront();
		}
	};

	typedef std::shared_ptr<FileQueue> FileQueue_ptr;
}
