#ifndef _DATAFILE_STRING_H_
#define _DATAFILE_STRING_H_

#include <vector>
#include <string>

namespace datafile
{
	namespace string
	{
		std::string to_lower(const std::string& string);
		std::string to_upper(const std::string& string);
		std::string fuse(const std::vector<std::string> vector, char separator);
		size_t separate(const std::string& string, std::vector<std::string>& vector, const char separator);
	}
}

#endif // !_DATAFILE_STRING_H_
