#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

std::string gen_random(const size_t len)
{
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	std::string ret;

	ret.reserve(len);

	for (size_t i = 0; i < len; ++i)
	{
		ret[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return ret;
}

namespace FileQueueUnitTest
{
	TEST_CLASS(FileQueueUnitTest)
	{
	public:
		
		TEST_METHOD(DefaultCtor)
		{
			const bool overwrite = true;

			auto fifo = std::make_unique<FileQueue>("queue.cache", overwrite);

			/* const int samples = 10000;
			const auto t1 = std::chrono::high_resolution_clock::now();

			for (int i = 0; i < samples; ++i)
			{
			fifo->Push("Hello");
			}

			const auto t2 = std::chrono::high_resolution_clock::now();

			const auto dur = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count());

			std::cout << "Writing " << samples << " samples took " << dur / 1000 << " ms. " << dur / 1000 / samples << " ms per sample";*/

			fifo->Push("Hello");
			fifo->Push("world");

			fifo->Push("Mistake!");
		}

		TEST_METHOD(PushPop)
		{
			std::vector<std::string> items;

			for (int i = 0; i < 10000; ++i)
			{
				items.push_back(
					gen_random((rand() % 1024) + 512)
				);
			}

			const auto overwrite = true;

			FileQueue queue("test.cache", overwrite);

			for (auto& item : items)
			{
				queue.Push(item);
			}

			for (auto iter = items.rend(); iter != items.rbegin(); ++iter)
			{
				const auto& item = *iter;
				const auto& data = queue.Pop();

				Assert::AreEqual(item, data);
			}

			Assert::IsTrue(queue.Empty());
		}

		TEST_METHOD(Empty)
		{
			const auto overwrite = true;

			FileQueue queue("test.cache", overwrite);

			Assert::IsTrue(queue.Empty());

			queue.Push("Hello world");
			queue.Push("Hello world");
			queue.Push("Hello world");

			Assert::IsFalse(queue.Empty());

			queue.Clear();

			Assert::IsTrue(queue.Empty());
		}

		TEST_METHOD(Length)
		{
			const auto overwrite = true;

			FileQueue queue("test.cache", overwrite);

			Assert::AreEqual(static_cast<size_t>(0), queue.Length());

			queue.Push("Hello world");
			queue.Push("Hello world");
			queue.Push("Hello world");
			queue.Push("Hello world");

			Assert::AreEqual(static_cast<size_t>(4), queue.Length());

			queue.Pop();

			Assert::AreEqual(static_cast<size_t>(3), queue.Length());

			queue.Pop();
			queue.Pop();
			queue.Pop();

			Assert::AreEqual(static_cast<size_t>(0), queue.Length());

			queue.Pop();

			Assert::AreEqual(static_cast<size_t>(0), queue.Length());
		}

		TEST_METHOD(Clear)
		{
			const auto overwrite = true;

			FileQueue queue("test.cache", overwrite);

			queue.Push("Hello world");
			queue.Push("Hello world");
			queue.Push("Hello world");
			queue.Push("Hello world");

			Assert::AreEqual(static_cast<size_t>(4), queue.Length());

			queue.Clear();

			Assert::AreEqual(static_cast<size_t>(0), queue.Length());
			Assert::IsTrue(queue.Empty());
		}
	};
}
