#include "datafile/d_string.h"

namespace dlh
{
	namespace string
	{
		std::string to_lower(const std::string& string)
		{
			std::string output;

			for (auto c : string)
			{
				output.push_back(std::tolower(c));
			}

			return output;
		}

		std::string to_upper(const std::string& string)
		{
			std::string output;

			for (auto c : string)
			{
				output.push_back(std::toupper(c));
			}

			return output;
		}

		std::string fuse(const std::vector<std::string> vector, char separator)
		{
			std::string output;

			for (auto h : vector)
			{
				if (output.size() > 0)
				{
					output.push_back(separator);
				}
				output.append(string::to_upper(h));
			}

			return output;
		}

		size_t separate(const std::string& string, std::vector<std::string>& vector, const char separator)
		{
			size_t x = string.find_first_of(separator);

			if (x != std::string::npos)
			{
				vector.push_back(string.substr(0, x));
				separate(string.substr(x + 1, string.size()), vector, separator);
			}
			else
			{
				vector.push_back(string);
			}

			return vector.size();
		}
	}
}
