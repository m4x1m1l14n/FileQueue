#include "pch.h"
#include "CppUnitTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = std::filesystem;


/*
TODO Load corrupted cache (lower size than header size, corrupted data | header etc)
*/


namespace UnitTests
{
	TEST_CLASS(FileStackUnitTest)
	{
	public:
		TEST_METHOD(DefaultCtor)
		{
			const bool overwrite = true;

			auto stack = std::make_unique<FileStack>("stack.cache", overwrite);

			Assert::IsTrue(stack->Empty());
			Assert::AreEqual(static_cast<size_t>(0), stack->Length());
		}

		TEST_METHOD(PopOnFreshStack)
		{
			const bool overwrite = true;

			auto stack = std::make_unique<FileStack>("stack.cache", overwrite);

			const auto& data = stack->Pop();

			Assert::AreEqual(data, std::string());
		}

		TEST_METHOD(StackReOpen)
		{
			bool overwrite = true;

			auto stack = std::make_unique<FileStack>("stack.cache", overwrite);

			std::vector<std::string> items;

			for (int i = 0; i < 100; ++i)
			{
				items.push_back(
					gen_random((static_cast<size_t>(rand()) % 4096) + 2048)
				);
			}

			overwrite = false;

			stack = std::make_unique<FileStack>("stack.cache", overwrite);

			for (auto iter = items.rbegin(); iter != items.rend(); ++iter)
			{
				const auto& item = *iter;
				const auto& data = stack->Pop();

				Assert::AreEqual(item, data);
			}
		}

		TEST_METHOD(PushEmptyData)
		{
			bool overwrite = true;

			auto stack = std::make_unique<FileStack>("stack.cache", overwrite);

			stack->Push("");

			const auto& data = stack->Pop();

			Assert::AreEqual(std::string(""), data);
		}

		TEST_METHOD(PushPop)
		{
			std::vector<std::string> items;

			for (int i = 0; i < 10000; ++i)
			{
				items.push_back(
					gen_random((static_cast<size_t>(rand()) % 4096) + 2048)
				);
			}

			const auto overwrite = true;

			FileStack stack("stack.cache", overwrite);

			for (auto& item : items)
			{
				stack.Push(item);
			}

			for (auto iter = items.rbegin(); iter != items.rend(); ++iter)
			{
				const auto& item = *iter;
				const auto& data = stack.Pop();

				Assert::AreEqual(item, data);
			}

			Assert::IsTrue(stack.Empty());
		}

		TEST_METHOD(Empty)
		{
			const auto overwrite = true;

			FileStack stack("stack.cache", overwrite);

			Assert::IsTrue(stack.Empty());

			stack.Push("Hello world");
			stack.Push("Hello world");
			stack.Push("Hello world");

			Assert::IsFalse(stack.Empty());

			stack.Pop();
			stack.Pop();
			stack.Pop();

			Assert::IsTrue(stack.Empty());

			stack.Push("Hello world");
			stack.Push("Hello world");
			stack.Push("Hello world");

			Assert::IsFalse(stack.Empty());

			stack.Pop();
			stack.Pop();
			stack.Pop();

			Assert::IsTrue(stack.Empty());

			stack.Pop();

			Assert::IsTrue(stack.Empty());
		}

		TEST_METHOD(Length)
		{
			const auto overwrite = true;

			FileStack stack("stack.cache", overwrite);

			Assert::AreEqual(static_cast<size_t>(0), stack.Length());

			stack.Push("Hello world");
			stack.Push("Hello world");
			stack.Push("Hello world");
			stack.Push("Hello world");

			Assert::AreEqual(static_cast<size_t>(4), stack.Length());

			stack.Pop();

			Assert::AreEqual(static_cast<size_t>(3), stack.Length());

			stack.Pop();
			stack.Pop();
			stack.Pop();

			Assert::AreEqual(static_cast<size_t>(0), stack.Length());

			stack.Pop();

			Assert::AreEqual(static_cast<size_t>(0), stack.Length());
		}

		TEST_METHOD(Clear)
		{
			const auto overwrite = true;

			FileStack stack("stack.cache", overwrite);

			stack.Push("Hello world");
			stack.Push("Hello world");
			stack.Push("Hello world");
			stack.Push("Hello world");

			Assert::AreEqual(static_cast<size_t>(4), stack.Length());

			stack.Clear();

			Assert::AreEqual(static_cast<size_t>(0), stack.Length());
			Assert::IsTrue(stack.Empty());
		}
	};
}
