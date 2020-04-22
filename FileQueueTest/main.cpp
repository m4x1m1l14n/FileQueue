#include <iostream>

#include <FileQueue/FileQueue.hpp>


using namespace m4x1m1l14n;


int main(void)
{
	const auto overwrite = true;

	auto stack = std::make_shared<FileStack>("stack.cache", overwrite);

	stack->Push("Hello world 1");
	stack->Push("Hello world 2");
	
	std::cout << "Pop: " << stack->Pop() << std::endl;
	std::cout << "Pop: " << stack->Pop() << std::endl;
	std::cout << "Pop: " << stack->Pop() << std::endl;

	return 0;
}