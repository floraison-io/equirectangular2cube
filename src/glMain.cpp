#pragma once
#include <iostream>
#include <string>
void writeHDRCubemap(std::string srcPath, std::string outPath, int size);
int main() {
	std::string srcPath, outPath;
	int size;
	std::cout << "srcFile: ";
	std::cin >> srcPath;
	std::cout << "outDirectory: ";
	std::cin >> outPath;
	std::cout << "size: ";
	std::cin >> size;
	try
	{
		writeHDRCubemap(srcPath, outPath, size);
	}
	catch (std::runtime_error e)
	{
		std::cerr << e.what() << std::endl;
	}
}