#ifndef II887522_RAIN_IO_H
#define II887522_RAIN_IO_H

#include <vector>
#include <string>
#include <fstream>

using std::vector;
using std::string;
using std::ifstream;

namespace ii887522::Rain
{
	vector<char> readFile(const string& fileName);
	unsigned int fileSize(ifstream& fileStream);
}

#endif