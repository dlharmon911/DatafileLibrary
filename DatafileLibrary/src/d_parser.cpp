#include <allegro5/allegro5.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_physfs.h>
#include <physfs.h>
#include "datafile/d_path.h"
#include "datafile/d_string.h"
#include "datafile/d_config.h"
#include "datafile/d_system.h"
#include "datafile/d_data.h"
#include "datafile/d_object.h"
#include "datafile/d_parser.h"
#include "datafile.h"

namespace dlh
{
	namespace datafile
	{
		namespace parser
		{
			bool m_initialized = false;

			void register_default_object_types()
			{
				if (!m_initialized)
				{
					datafile::object::register_type(datafile::object::type_t::bitmap, "bitmap", datafile::object::bitmap_parser);
					datafile::object::register_type(datafile::object::type_t::font, "font", datafile::object::font_parser);
					datafile::object::register_type(datafile::object::type_t::text, "text", datafile::object::text_parser);
					datafile::object::register_type(datafile::object::type_t::datafile, "datafile", datafile::object::datafile_parser);
				}
				m_initialized = true;
			}

			std::vector<std::string> m_path;

			int32_t process_shortcuts(const dson_t& dson, config_t& config, const std::string& prefix = "")
			{
				for (auto i = dson.cbegin(); i != dson.cend(); ++i)
				{
					std::string key = i.key();
					if (key[0] == '$')
					{
						std::string name = key.substr(1, key.length() - 1);
						std::string value = i->get_string();

						if (value.size() == 0)
						{
							return -1;
						}

						if (prefix.size() == 0)
						{
							config[name].set_string(value);
						}
						else
						{

							config.get_property(prefix)[name].set_string(value);
						}
					}
					else
					{
						std::string _prefix = prefix;
						if (_prefix.size() > 0)
						{
							_prefix.push_back('.');
						}

						if (process_shortcuts(dson[key], config, _prefix + key) < 0)
						{
							return -1;
						}
					}
				}

				return 0;
			}

			std::shared_ptr<datafile_t> parse(const dson_t& dson, config_t& config)
			{
				std::shared_ptr<datafile_t> rv;

				auto i = dson.cbegin();
				dson_t output;

				if (process_shortcuts(*i, config) == 0)
				{
					data_t data((*i), config);

					rv = std::static_pointer_cast<datafile_t>(object::datafile_parser(data));
				}

				return rv;
			}

			std::shared_ptr<datafile_t> parse(const std::string& filename, const char sListSep)
			{
				dson_t dson;
				std::string working_path;
				std::string index_path;
				std::string file_name;
				config_t config;

				register_default_object_types();

				if (al_filename_exists(filename.c_str()))
				{
					if (dson_t::read(dson, filename))
					{
						if (dson.get_children_count() == 1)
						{
							return parse(dson, config);
						}
					}
				}

				return nullptr;
			}

			bool write_header_object(ALLEGRO_FILE* pfile, const dson_t& dson, std::vector<std::string>& header, const std::string& name, int32_t& n)
			{
				for (auto i = dson.cbegin(); i != dson.cend(); ++i)
				{
					std::string key = i.key();

					if (key[0] == '$') continue;

					header.push_back(key);
					std::string out = "constexpr auto " + string::fuse(header, '_') + " = " + std::to_string(n) + ';';
					while (out.size() < 51) out.push_back(' ');
					out.append("/* ");
					out.append(string::to_upper(name));
					out.append(" */\n");
					al_fputs(pfile, out.c_str());
					header.pop_back();
					++n;
				}

				return true;
			}

			bool write_header_datafile(ALLEGRO_FILE* pfile, const dson_t& dson, std::vector<std::string>& header, int32_t& n)
			{
				for (auto i = dson.cbegin(); i != dson.cend(); ++i)
				{
					std::string key = i.key();

					if (key[0] == '$') continue;

					header.push_back(key);
					write_header_object(pfile, dson[key], header, key, n);
					header.pop_back();

					if (key == "dlh")
					{
						int32_t n = 0;
						const dson_t& dlh = dson[key];

						for (auto j = dlh.cbegin(); j != dlh.cend(); ++j)
						{
							header.push_back(j.key());
							write_header_datafile(pfile, dlh, header, n);
							header.pop_back();
						}
					}
				}

				return true;
			}

			bool write_header(const dson_t& dson, const std::string& input_text_filename, const std::string& output_header_filename)
			{
				bool rv = false;
				std::vector<std::string> header;
				ALLEGRO_FILE* pfile = nullptr;

				if (dson.get_children_count() == 1)
				{
					std::string output = path::make_canonical(output_header_filename);

					pfile = al_fopen(output.c_str(), "wb");

					if (pfile)
					{
						int32_t n = 0;

						al_fputs(pfile, "/* Datafile object indexes */\n");
						al_fputs(pfile, "/* Index File: ");
						al_fputs(pfile, input_text_filename.c_str());
						al_fputs(pfile, " */\n");
						al_fputs(pfile, "/* Date: ");
						al_fputs(pfile, system::timestamp().c_str());
						al_fputs(pfile, " */ \n");
						al_fputs(pfile, "/* Do not hand edit! */\n\n");

						auto it = dson.cbegin();
						std::string key = it.key();

						header.push_back(key);

						write_header_datafile(pfile, dson[key], header, n);

						header.pop_back();

						al_fputs(pfile, "\n\n");

						al_fclose(pfile);

						rv = true;
					}
				}

				return rv;
			}
		}
	}

	bool al_generate_header_file(const char* manifest_filename, const char* header_filename, const char sListSep)
	{
		bool rv = false;
		dlh::dson_t dson;
		std::string filepath;
		std::string base;
		std::string ext;
		std::string path;
		bool archive = false;

		filepath = dlh::path::make_canonical(manifest_filename);
		dlh::path::split_filepath(filepath, path, base, ext);

		const PHYSFS_ArchiveInfo** i = nullptr;
		for (i = PHYSFS_supportedArchiveTypes(); *i != NULL; i++)
		{
			if (dlh::string::to_upper(ext) == (*i)->extension)
			{
				archive = true;
				break;
			}
		}

		if (archive)
		{
			const ALLEGRO_FILE_INTERFACE* file_interface = al_get_new_file_interface();

			if (PHYSFS_mount(manifest_filename, NULL, 1))
			{
				al_set_physfs_file_interface();

				rv = dlh::dson_t::read(dson, "index.ini", sListSep);

				PHYSFS_unmount(manifest_filename);
			}

			al_set_new_file_interface(file_interface);
		}
		else
		{
			rv = dlh::dson_t::read(dson, manifest_filename, sListSep);
		}

		if (rv)
		{
			rv = dlh::datafile::parser::write_header(dson, manifest_filename, header_filename);
		}

		return rv;
	}
}



