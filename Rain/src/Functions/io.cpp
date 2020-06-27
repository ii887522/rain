#include "io.h"
#include <vector>
#include <string>
#include <fstream>

using std::vector;
using std::string;
using std::ifstream;
using std::ios;

namespace ii887522::Rain
{
	vector<char> readFile(const string& fileName)
	{
		ifstream file{ fileName, ios::binary };	
		vector<char> content(fileSize(file));
		file.read(content.data(), content.size());
		return content;
	}

	unsigned int fileSize(ifstream& fileStream)
	{
		fileStream.seekg(0, ios::end);
		const auto size{ fileStream.tellg() };
		fileStream.seekg(0, ios::beg);
		return static_cast<unsigned int>(size);
	}
}