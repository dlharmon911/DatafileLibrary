#include <allegro5/allegro5.h>
#include "datafile\d_path.h"

namespace datafile
{
	namespace path
	{
		bool set_working_directory()
		{
			ALLEGRO_PATH* base_path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);

			if (base_path)
			{
				al_make_path_canonical(base_path);
				al_change_directory(al_path_cstr(base_path, ALLEGRO_NATIVE_PATH_SEP));
				al_destroy_path(base_path);
				return true;
			}

			return false;
		}

		std::string get_file_part(const std::string& filepath)
		{
			std::string out;
			ALLEGRO_PATH* path = al_create_path(filepath.c_str());
			if (path)
			{
				out = al_get_path_filename(path);
				al_destroy_path(path);
			}
			return out;
		}

		std::string get_path_part(const std::string& filepath)
		{
			std::string filename = get_file_part(filepath);
			if (filepath.size() > filename.size())
			{

			}

			return filepath.substr(0, filepath.length() - filename.size());
		}

		std::string make_canonical(const std::string& str)
		{
			std::string out;

			for (auto c : str)
			{
				if (c == '/' || c == '\\')
				{
					out.push_back(ALLEGRO_NATIVE_PATH_SEP);
				}
				else
				{
					out.push_back(c);
				}
			}

			return out;
		}

		std::string get_extension_part(const std::string& filepath)
		{
			size_t size = filepath.find_last_of('.');

			if (size == std::string::npos || (size + 1) == filepath.size())
			{
				return "";
			}

			return filepath.substr(size + 1);
		}

		size_t find_all_files(const std::string& filepath, const std::string& ext, std::vector<std::string>& vec)
		{
			size_t size = vec.size();
			ALLEGRO_FS_ENTRY* entry = al_create_fs_entry(filepath.c_str());

			if (entry)
			{
				ALLEGRO_FS_ENTRY* next = nullptr;

				if (!(al_get_fs_entry_mode(entry) & ALLEGRO_FILEMODE_ISDIR))
				{
					return 0;
				}

				if (!al_open_directory(entry))
				{
					return 0;
				}

				while (true)
				{
					if (!(next = al_read_directory(entry))) break;

					if (al_get_fs_entry_mode(next) & ALLEGRO_FILEMODE_ISFILE)
					{
						std::string filename;
						std::string extension;

						ALLEGRO_PATH* path = al_create_path(al_get_fs_entry_name(next));
						if (!path)
						{
							break;
						}

						filename = al_get_path_filename(path);
						extension = al_get_path_extension(path);
						al_destroy_path(path);

						if (extension.size() > 0 && extension[0] == '.')
						{
							extension = extension.substr(1, extension.size() - 1);
						}

						if (ext == "*" || ext == extension)
						{
							vec.push_back(path::get_file_part(al_get_fs_entry_name(next)));
						}
					}

					al_destroy_fs_entry(next);
				}

				al_close_directory(entry);
				al_destroy_fs_entry(entry);
			}
			return vec.size() - size;
		}

		void split_filepath(const std::string& path, std::string& dir, std::string& basename, std::string& extension)
		{
			std::string ptemp = make_canonical(path);

			std::string filename = get_file_part(ptemp);
			size_t pos = filename.find_last_of('.');

			dir.clear();
			basename.clear();
			extension.clear();

			if (pos > 0)
			{
				basename = filename.substr(0, pos);

				if (pos < (filename.size() - 1))
				{
					extension = filename.substr(pos + 1);
				}
			}

			if (ptemp.size() > filename.size())
			{
				dir = ptemp.substr(0, ptemp.size() - filename.size());
			}
		}
	}
}