#ifndef _DATAFILE_CONFIG_H_
#define _DATAFILE_CONFIG_H_

#include "d_dson.h"

namespace dh
{
	using config_t = dson_t;

	namespace config
	{
		int32_t read(const std::string& filename, config_t& config_file);
		int32_t write(const std::string& filename, config_t& config_file);
		int32_t expand_string(const config_t& config_file, const std::string& name, std::string& output);

		std::string get_string(config_t& config_file, const std::string& name, const std::string& default_value);
		void set_string(config_t& config_file, const std::string& name, const std::string& value);

		template <typename T>
		T get_as(config_t& config_file, const std::string& name, const T default_value) = delete;
		template <> int32_t get_as(config_t& config_file, const std::string& name, const int32_t default_value);
		template <> double get_as(config_t& config_file, const std::string& name, const double default_value);
		template <> bool get_as(config_t& config_file, const std::string& name, const bool default_value);

		template <typename T>
		void set_as(config_t& config_file, const std::string& name, const T value) = delete;
		template <> void set_as(config_t& config_file, const std::string& name, int32_t value);
		template <>	void set_as(config_t& config_file, const std::string& name, double value);
		template <> void set_as(config_t& config_file, const std::string& name, bool value);
	}
}

#endif // !_DATAFILE_CONFIG_H_
