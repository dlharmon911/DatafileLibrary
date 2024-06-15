#ifndef _DATAFILE_PATH_H_
#define _DATAFILE_PATH_H_

#include <vector>
#include <string>

namespace datafile
{
	namespace path
	{
		bool set_working_directory();
		std::string get_path_part(const std::string& filepath);
		std::string get_file_part(const std::string& filepath);
		std::string get_extension_part(const std::string& filepath);
		size_t find_all_files(const std::string& filepath, const std::string& ext, std::vector<std::string>& vec);
		void split_filepath(const std::string& path, std::string& dir, std::string& basename, std::string& extension);
		std::string make_canonical(const std::string& str);
	}
}

#endif // !_DATAFILE_PATH_H_
