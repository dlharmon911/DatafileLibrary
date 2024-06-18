#include "datafile/d_string.h"
#include "datafile/d_config.h"

namespace dlh
{
	namespace config
	{
		int32_t read(const std::string& filename, config_t& config_file)
		{
			dson_t manifest;

			if (!dson_t::read(manifest, filename))
			{
				return -1;
			}

			config_file.clear();

			if (!manifest.contains("manifest"))
			{
				return -1;
			}

			dson_t& _manifest = manifest["manifest"];

			if (!_manifest.contains("path"))
			{
				return -1;
			}

			dson_t& _manifest_path = _manifest["path"];

			for (auto it = _manifest_path.cbegin(); it != _manifest_path.cend(); ++it)
			{
				std::string key = it.key();
				std::string input = it->get_string();

				config_file["path"][key].set_string(input);
			}

			if (!_manifest.contains("file"))
			{
				return -1;
			}

			dson_t& _manifest_file = _manifest["file"];

			for (auto it = _manifest_file.begin(); it != _manifest_file.end(); ++it)
			{
				std::string key = it.key();
				std::string input = it->get_string();
				std::string filename;
				dson_t data;

				config_file["file"][key].set_string(input);

				if (config::expand_string(config_file, input, filename) < 0)
				{
					return -1;
				}

				if (!dson_t::read(data, filename))
				{
					return -1;
				}
				auto dit = data.begin();
				config_file[dit.key()] = (*dit);
			}

			return 0;
		}

		int32_t write(const std::string& filename, config_t& config_file)
		{
			for (auto i = config_file.begin(); i != config_file.end(); ++i)
			{
				std::string key = i.key();

				if (!(key == "path" || key == "file"))
				{
					std::string file_shortcut = std::string("$(file.") + key + ")";
					std::string file_name;
					if (expand_string(config_file, file_shortcut, file_name) == -1)
					{
						return -1;
					}

					dson_t temp;
					temp[key] = *i;

					if (!dson_t::write(temp, file_name))
					{
						return -1;
					}
				}
			}

			return 0;
		}

		int32_t expand_string(const config_t& config_file, const std::string& name, std::string& output)
		{
			bool has_shortcut = false;
			size_t i = 0;

			output.clear();

			while (i < name.size())
			{
				if (name[i] == '$')
				{
					has_shortcut = true;
					std::string s;

					++i;
					if (name[i] != '(')
					{
						return -1;
					}
					++i;
					while (name[i] != ')')
					{
						if (i == name.size())
						{
							return -1;
						}
						s.push_back(name[i]);
						++i;
					}
					dson_t sk = config_file.get_property(s);
					std::string shortcut = sk.get_string();
					if (shortcut.size() <= 0)
					{
						return -1;
					}

					output.append(shortcut);
				}
				else
				{
					output.push_back(name[i]);
				}
				++i;
			}

			if (has_shortcut)
			{
				std::string input = output;
				output.clear();
				return expand_string(config_file, input, output);
			}

			return 0;
		}

		std::string get_string(config_t& config_file, const std::string& name, const std::string& default_value)
		{
			std::string expansion;
			
			if (expand_string(config_file, name, expansion) == 0)
			{
				dson_t& d = config_file.get_property(expansion);

				if (d.empty())
				{
					d.set_string(default_value);
				}

				if (expand_string(config_file, d.get_string(), expansion) < 0)
				{
					return std::string();
				}
			}
			return expansion;
		}

		void set_string(config_t& config_file, const std::string& name, const std::string& value)
		{
			config_file.get_property(name).set_string(value);
		}

		template <>
		int32_t get_as(config_t& config_file, const std::string& name, const int32_t default_value)
		{
			std::string str = config::get_string(config_file, name, std::to_string(default_value));
			return std::atoi(str.c_str());
		}

		template <>
		double get_as(config_t& config_file, const std::string& name, const double default_value)
		{
			std::string str = config::get_string(config_file, name, std::to_string(default_value));
			return std::atof(str.c_str());
		}

		template <>
		bool get_as(config_t& config_file, const std::string& name, const bool default_value)
		{
			std::string str = config::get_string(config_file, name, (default_value ? "true" : "false"));
			return string::to_lower(str) == "true";
		}

		template <>
		void set_as(config_t& config_file, const std::string& name, int32_t value)
		{
			config::set_string(config_file, name, std::to_string(value));
		}

		template <>
		void set_as(config_t& config_file, const std::string& name, double value)
		{
			config::set_string(config_file, name, std::to_string(value));
		}

		template <>
		void set_as(config_t& config_file, const std::string& name, bool value)
		{
			config::set_string(config_file, name, std::string(value ? "true" : "false"));
		}
	}
}
