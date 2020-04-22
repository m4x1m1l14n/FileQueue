#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = std::filesystem;

/*
TODO Load corrupted cache (lower size than header size, corrupted data | header etc)
*/

namespace UnitTests
{
	TEST_CLASS(FileQueueUnitTest)
	{
	public:
		/*TEST_METHOD(FilePointersTest)
		{
			fs::path path("output.text");

			std::fstream file;

			file.open(path, std::fstream::trunc | std::fstream::out | std::fstream::in | std::fstream::binary);
			
			Assert::IsTrue(file.is_open());

			std::streamoff put_pointer = file.tellp();
			std::streamoff get_pointer = file.tellg();

			Assert::AreEqual(static_cast<std::int64_t>(put_pointer), static_cast<std::int64_t>(get_pointer));

			const std::string data = "Hello world!";

			file.write(data.c_str(), static_cast<std::streamsize>(data.length()));

			put_pointer = file.tellp();
			get_pointer = file.tellg();

			Assert::AreNotEqual(static_cast<std::int64_t>(put_pointer), static_cast<std::int64_t>(get_pointer));

			put_pointer = put_pointer - static_cast<std::streamoff>(data.length());

			Assert::AreEqual(static_cast<std::int64_t>(put_pointer), static_cast<std::int64_t>(get_pointer));

			fs::remove(path);
		}*/
		
		TEST_METHOD(DefaultCtor)
		{
			const bool overwrite = true;

			auto queue = std::make_unique<FileQueue>("queue.cache", overwrite);

			/* const int samples = 10000;
			const auto t1 = std::chrono::high_resolution_clock::now();

			for (int i = 0; i < samples; ++i)
			{
				fifo->Push("Hello");
			}

			const auto t2 = std::chrono::high_resolution_clock::now();

			const auto dur = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count());

			std::cout << "Writing " << samples << " samples took " << dur / 1000 << " ms. " << dur / 1000 / samples << " ms per sample";*/
		}
	};
}
